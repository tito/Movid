from OpenGL.GL import *
from pymt import *
from pyot import *

def spline_4p( t, p_1, p0, p1, p2 ):
    # wikipedia Catmull-Rom -> Cubic_Hermite_spline
    # 0 -> p0,  1 -> p1,  1/2 -> (- p_1 + 9 p0 + 9 p1 - p2) / 16
    return (
          t*((2-t)*t - 1)   * p_1
        + (t*t*(3*t - 5) + 2) * p0
        + t*((4 - 3*t)*t + 1) * p1
        + (t-1)*t*t         * p2 ) / 2

def make_spline(origin, destination, controll_points, resolution=10):
    P = [origin, origin] + controll_points + [destination, destination]
    p = []
    for j in range( 1, len(P)-2 ):  # skip the ends
        for t in xrange( resolution ):  # t: 0 .1 .2 .. .9
            x = spline_4p( float(t)/resolution, P[j-1][0], P[j][0], P[j+1][0], P[j+2][0] )
            y = spline_4p( float(t)/resolution, P[j-1][1], P[j][1], P[j+1][1], P[j+2][1] )
            p.extend((x,y))
    return p



class Singleton(type):
    def __init__(cls, name, bases, dict):
        super(Singleton, cls).__init__(name, bases, dict)
        cls.instance = None
 
    def __call__(cls, *args, **kw):
        if cls.instance is None:
            cls.instance = super(Singleton, cls).__call__(*args, **kw)
        return cls.instance

class MovidGUI(MTScatterPlane):
    
    __metaclass__ = Singleton
    
    def __init__(self, daemon):
        super(MovidGUI, self).__init__()
        self.daemon = daemon

        for module in daemon.modules.values():
            self.add_widget(ModuleNode(module, size=(200,150)) )
            
    def new_module_btn(self, m, *args):
        module = self.daemon.add_module(m)
        self.add_widget(ModuleNode(module, size=(200,150)) )
        
        
        
class ConnectionButton(MTButton):
    def __init__(self, **kwargs):
        kwargs.setdefault('anchor_x', 'left')
        kwargs.setdefault('anchor_y', 'bottom')
        kwargs.setdefault('color', (1,1,1))
        super(ConnectionButton, self).__init__(**kwargs)
        
        self.img = Image('arrow.png')
        self.height = self.img.height
        self.width = self.img.width + self.label_obj.width
        self.userdata = {}
        self.userdata['target'] = None
        
        self.connection = None
        
    def on_touch_down(self, touch):
        touch.userdata['target'] = None
        touch.userdata['origin'] = None
        if self.collide_point(*touch.pos):
            touch.grab(self)
            touch.userdata['origin'] = self
            self.connection = touch
            return True
         
    def on_touch_move(self, touch):
        if touch.grab_current == self and self == touch.userdata['target']:
            if  not self.collide_point(*touch.pos):
                touch.userdata['target'] = None
        elif self.collide_point(*touch.pos) and touch.userdata['origin'] != self:
            touch.userdata['target'] = self
            touch.grab(self)
            
        
            
    def on_touch_up(self, touch):
        if touch.userdata['target'] == self:
            self.userdata['target'] = self
            touch.userdata['origin'].connection = self
        
            
    def attach_pos(self):
        return self.x , self.y + self.img.height/2
    def attach_offset(self):
        return -1
            
    def draw(self):
        if self.connection:
            src = self.attach_pos()
            dst = self.to_widget(*self.connection.pos)
            
            set_color(.9,.6,0)
            if self.connection.userdata['target']:
                set_color(0,1,0)
            if self.connection.userdata['target'] is ConnectionButton :
                set_color(1,1,0)
                src = self.attach_pos()
                dst = self.to_widget(*self.connection.to_window(self.connection.pos))
                

            d = self.attach_offset() * Vector(src).distance(dst) *0.2
            spline = make_spline(src, dst, [(src[0]+d, src[1]), (dst[0]-d, dst[1])])
            drawLine( spline , width=4)
            


class InputButton(ConnectionButton):
    def draw(self):
        self.img.pos = self.pos
        self.img.draw()
        self.label_obj.pos = self.pos
        self.label_obj.x += self.img.width
        self.label_obj.draw()
        super(InputButton, self).draw()
        
class OutputButton(ConnectionButton):
    def draw(self):
        self.label_obj.pos = self.pos
        self.label_obj.draw()    
        self.img.pos = self.pos
        self.img.x += self.label_obj.width
        self.img.draw()
        super(OutputButton, self).draw()
        
    def attach_pos(self):
        return (self.x + self.img.width + self.label_obj.width, self.y + self.img.height/2)
    def attach_offset(self):
        return 1


class ModuleNode(MTScatterWidget):
    
    def __init__(self, module, **kwargs):
        super(ModuleNode, self).__init__(**kwargs)
        self.module = module
        
        input_box = MTBoxLayout(orientation='vertical')
        for inp in self.module.inputs:
            btn = InputButton(label=inp['name'])
            input_box.add_widget(btn)
        self.add_widget(input_box)
        
        output_box = MTBoxLayout(orientation='vertical')
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
        



    

daemon = MovidDaemon()

if not len(daemon.modules):
    daemon.add_module("Camera", "cam")
    daemon.add_module("ImageDisplay", "display")



root = MTWidget()
gui = MovidGUI(daemon)
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
