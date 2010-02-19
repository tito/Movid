from OpenGL.GL import *
from pymt import *
from pyot import *


class ModuleNode(MTScatterWidget):
    def __init__(self, module, **kwargs):
        super(ModuleNode, self).__init__(**kwargs)
        self.module = module
        
        self.label = Label("%s (%s)"% (self.module.id, self.module.type))
        self.property_labels = {}
        for key, val in self.module.properties.items():
            label = Label(key+"="+str(val))
            self.property_labels[key] = label
            
    def update_info(self):
        self.label.text = "%s (%s)"% (self.module.id, self.module.type)
        for key, val in self.module.properties.items():
            self.property_labels[key].text = key+"="+str(val)
            
    def draw(self):
        set_color(0,0,0,0.5)
        drawRectangle(size=self.size)
        with gx_matrix:
            #start at the top
            glTranslate(0, self.height-self.label.height-5, 0)
            self.label.draw()
            
            drawLine([0,0,self.width, 0], width=1)
            
            for label in self.property_labels.values():
                glTranslate(0, -(label.height+2), 0)
                label.draw()
            


class OpenTrackerGUI(MTScatterPlane):
    def __init__(self, daemon):
        super(OpenTrackerGUI, self).__init__()
        self.daemon = daemon
        
        
        for module in daemon.modules.values():
            self.add_widget(ModuleNode(module, size=(200,150), pos=(100,200)) )
            

            
    def new_module_btn(self, m, *args):
        module = self.daemon.add_module(m)
        self.add_widget(ModuleNode(module, size=(200,150), pos=(100,200)) )
        

daemon = OpenTrackerDaemon()

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