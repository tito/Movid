from OpenGL.GL import *
from pymt import *
from pyot import *


class Singleton(type):
    def __init__(cls, name, bases, dict):
        super(Singleton, cls).__init__(name, bases, dict)
        cls.instance = None
 
    def __call__(cls, *args, **kw):
        if cls.instance is None:
            cls.instance = super(Singleton, cls).__call__(*args, **kw)
        return cls.instance

class OpenTrackerGUI(MTScatterPlane):
    
    __metaclass__ = Singleton
    
    def __init__(self, daemon):
        super(OpenTrackerGUI, self).__init__()
        self.daemon = daemon

        for module in daemon.modules.values():
            self.add_widget(ModuleNode(module, size=(200,150), pos=(100,200)) )
            
    def new_module_btn(self, m, *args):
        module = self.daemon.add_module(m)
        self.add_widget(ModuleNode(module, size=(200,150), pos=(100,200)) )
        
        
        
class ConnectionButton(MTButton):
    def __init__(self, **kwargs):
        kwargs.setdefault('anchor_x', 'left')
        kwargs.setdefault('anchor_y', 'bottom')
        kwargs.setdefault('color', (1,1,1))
        super(ConnectionButton, self).__init__(**kwargs)
        
        self.img = Image('arrow.png')
        self.height = self.img.height
        self.width = self.img.width + self.label_obj.width

class InputButton(ConnectionButton):
    def draw(self):
        self.img.pos = self.pos
        self.img.draw()
        self.label_obj.pos = self.pos
        self.label_obj.x += self.img.width
        self.label_obj.draw()
        
class OutputButton(ConnectionButton):
    def draw(self):
        self.label_obj.pos = self.pos
        self.label_obj.draw()    
        self.img.pos = self.pos
        self.img.x += self.label_obj.width
        self.img.draw()        
        


class ModuleNode(MTScatterWidget):
    
    def __init__(self, module, **kwargs):
        super(ModuleNode, self).__init__(**kwargs)
        self.module = module
        
        input_box = MTBoxLayout()
        for inp in self.module.inputs:
            btn = InputButton(label=inp['name'])
            input_box.add_widget(btn)
        self.add_widget(input_box)
        
        output_box = MTBoxLayout()
        for out in self.module.outputs:
            btn = OutputButton(label=out['name'])
            output_box.add_widget(btn)
        output_box.x = self.width - output_box.content_width
        self.add_widget(output_box)
        
        self.label = Label("%s"% (self.module.id))
        self.property_labels = {}
        for key, val in self.module.properties.items():
            label = Label(key+"="+str(val))
            self.property_labels[key] = label
            
    def collide_point(self, x, y):
        local_coords = self.to_local(x, y)
        if local_coords[0] > 0 and local_coords[0] < self.width \
           and local_coords[1] > 0 and local_coords[1] < self.height:
            return True
        else:
            for c in self.children:
                if c.collide_point(*local_coords):
                    return True
            return False
            
            
    def update_info(self):
        self.label.text = "%s"% (self.module.id)
        for key, val in self.module.properties.items():
            self.property_labels[key].text = key+"="+str(val)
            
    def draw(self):
        set_color(0,0,0,0.5)
        drawRectangle(size=self.size)
        with gx_matrix:
            glTranslate(0, self.height-self.label.height-5, 0)
            self.label.draw()
            drawLine([0,0,self.width, 0], width=1)
        



    

daemon = OpenTrackerDaemon()

if not len(daemon.modules):
    daemon.add_module("Camera", "cam")
    daemon.add_module("ImageDisplay", "display")



root = MTWidget()
gui = OpenTrackerGUI(daemon)
toolbar = MTBoxLayout(orientation='vertical')

for m in daemon.available_modules:
    button = MTButton(label=m, size=(150,40))
    button.push_handlers( on_release=curry(gui.new_module_btn, m) )
    toolbar.add_widget(button)
toolbar.add_widget(MTLabel(label="Modules:"))




container = MTWidget()
container.add_widget(gui)
root.add_widget(container)
root.add_widget(toolbar)


runTouchApp(root)