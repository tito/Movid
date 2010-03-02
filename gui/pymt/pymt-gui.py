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

def make_spline(points, resolution, first_is_controll=False, last_is_controll=False):
    if not first_is_controll:
        points.insert(0,points[0])
    if not last_is_controll:
        points.append(points[-1])

    P = points
    p = []
    for j in range( 1, len(P)-2 ):  # skip the ends
        for t in xrange( resolution ):  # t: 0 .1 .2 .. .9
            x = spline_4p( float(t)/resolution, P[j-1][0], P[j][0], P[j+1][0], P[j+2][0] )
            y = spline_4p( float(t)/resolution, P[j-1][1], P[j][1], P[j+1][1], P[j+2][1] )
            p.extend((x,y))
    return p

def drawSpline(points, resolution=10, width=1):
    drawLine(make_spline(points, resolution), width=width)



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
        self.connections = []
        self.modules = {}
        for module in daemon.modules.values():
            self.modules[module.id] = ModuleNode(module, size=(200,150))
            self.add_widget(self.modules[module.id] )

    def new_module_btn(self, m, *args):
        module = self.daemon.add_module(m)
        self.modules[module.id] = ModuleNode(module, size=(200,150))
        self.add_widget(self.modules[module.id]  )

    def find_connection(self, window_pos):
        for mod in self.children:
            if mod.collide_point(*self.to_local(*window_pos)):
                for btn in mod.io_buttons:
                    if btn.collide_point(*btn.to_widget(*window_pos)):
                        return btn
        return False



class ConnectionButton(MTButton):
    def __init__(self, **kwargs):
        kwargs.setdefault('anchor_x', 'left')
        kwargs.setdefault('anchor_y', 'bottom')
        kwargs.setdefault('color', (1,1,1))
        super(ConnectionButton, self).__init__(**kwargs)

        self.index = kwargs['index']
        self.module = kwargs['module']
        self.label_obj.text = str(self.index)+":"+self.label_obj.text

        self.img = Image('arrow.png')
        self.height = self.img.height
        self.width = self.img.width + self.label_obj.width

        self.connection_line = None
        self.connection_color = (1,0,0)
        self.connection = None

    def draw(self):
        if self.connection_line:
            set_color(*self.connection_color)
            drawLine(self.connection_line, width=4)

    def build_line(self, src, dst):
        return src + dst

    def target_moded(self, x, y):
        self.build_line(self.attach_pos(), )

    def on_touch_down(self, touch):
        if self.collide_point(*touch.pos):
            touch.grab(self)
            return True

    def on_touch_move(self, touch):
        if touch.grab_current == self:
            self.connection_line = self.build_line(self.attach_pos(), touch.pos)
            target = MovidGUI().find_connection(self.to_window(*touch.pos))
            if self.is_valid_target(target):
                self.connection_color = (0,1,0)
            else:
                self.connection_color = (1,0,0)
            return True

    def on_touch_up(self, touch):
        if touch.grab_current == self:
            GUI = MovidGUI()
            target = GUI.find_connection(self.to_window(*touch.pos))
            if self.is_valid_target(target):
                self.connect(target)
                self.connection_line = None


class InputButton(ConnectionButton):
    def draw(self):
        self.img.pos = self.pos
        self.img.draw()
        self.label_obj.pos = self.pos
        self.label_obj.x += self.img.width
        self.label_obj.draw()
        super(InputButton, self).draw()
    def attach_pos(self):
        return self.x , self.y + self.img.height/2
    def attach_offset(self):
        return -0.3
    def is_valid_target(self, io_button):
        return isinstance(io_button, OutputButton)
    def connect(self, target):
        server = MovidGUI().daemon
        server.connect_module_by_index(target.module, target.index, self.module, self.index)
        target.parent.parent.connections = [(target.index, self.index, self.module)]

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
        return 0.3
    def is_valid_target(self, io_button):
        return isinstance(io_button, InputButton)
    def connect(self, target):
        server = MovidGUI().daemon
        server.connect_module_by_index(self.module, self.index, target.module, target.index)
        self.parent.parent.connections =[(self.index, target.index, target.module)]

class ModuleNode(MTScatterWidget):

    def __init__(self, module, **kwargs):
        super(ModuleNode, self).__init__(**kwargs)
        self.module = module

        self.connections = []
        self.io_buttons = []
        self.input_buttons = []
        self.output_buttons = []

        #add input connections
        input_box = MTBoxLayout(orientation='vertical')
        for inp in self.module.inputs:
            btn = InputButton(label=inp['name'], index=inp['index'], module=self.module.id,)
            self.io_buttons.append(btn)
            self.input_buttons.append(btn)
            input_box.add_widget(btn)
        self.add_widget(input_box)

        #add output connections
        output_box = MTBoxLayout(orientation='vertical')
        for out in self.module.outputs:
            btn = OutputButton(label=out['name'], module=self.module.id, index=out['index'])
            for indx, module_id in out['observers'].items():
                self.connections.append((out['index'], indx, module_id))
            self.io_buttons.append(btn)
            self.output_buttons.append(btn)
            output_box.add_widget(btn)
        output_box.x = self.width - output_box.width
        self.add_widget(output_box)

        #make name and property labels
        self.label = Label("%s"% (self.module.id))
        self.property_labels = {}
        for key, val in self.module.properties.items():
            label = Label(key+"="+str(val))
            self.property_labels[key] = label


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

            for l in self.property_labels.values():
                glTranslate(0, -l.height-2, 0)
                l.draw()
        for c in self.connections:
            src = self.output_buttons[int(c[0])]
            target = MovidGUI().modules[c[2]].input_buttons[int(c[1])]
            x,y = self.to_widget(*src.to_window(*src.attach_pos()))
            x1,y1 = self.to_widget(*target.to_window(*target.attach_pos()))
            set_color(0,1,0)
            drawLine([x,y,x1,y1], width=4)





if __name__ == "__main__":
    #Open Tracker Gui
    daemon = MovidDaemon()
    gui = MovidGUI(daemon)

    #add module buttons
    toolbar = MTBoxLayout(orientation='vertical')
    for m in daemon.available_modules:
        button = MTButton(label=m, size=(150,40))
        button.push_handlers( on_release=curry(gui.new_module_btn, m) )
        toolbar.add_widget(button)
    toolbar.add_widget(MTLabel(label="Modules:"))

    start_btn = MTButton(label="start", size=(150,100))
    def start_stop(*args):
        if start_btn.label == "start":
            daemon.start()
            start_btn.label = "stop"
        else:
            daemon.stop()
            start_btn.label = "start"
    start_btn.push_handlers(on_press=start_stop)
    toolbar.add_widget(start_btn)

    #add so that toolar stays on top
    root = MTWidget()
    container = MTWidget()
    container.add_widget(gui)
    root.add_widget(container)
    root.add_widget(toolbar)


    runTouchApp(root)