import json
import urllib

class otModulePropertiesProxy(object):
    def __init__(self, module_proxy, properties):
        self._module_proxy = module_proxy
        self.cache = properties

    def __getitem__(self, key):
        return self.cache[key]

    def __setitem__(self, key, val):
        self._module_proxy.set_property(key, val)
        self.cache[key] = val

    def keys(self):
        return self.cache.keys()

    def items(self):
        return self.cache.items()

    def __repr__(self):
        return str(self.cache)


class otModuleProxy(object):
    def __init__(self, daemon, id, type):
        self.daemon = daemon
        self._id = id
        self._type = type

        property_dict = self.daemon.describe(self.type)['properties']
        property_dict['id'] = id
        self.properties = otModulePropertiesProxy(self, property_dict)

    def _get_id(self):
        return self._id
    def _set_id(self, id):
        self.properties['id'] = id
        self.daemon.rename_module(self._id, id)
    id = property(_get_id, _set_id)

    def _get_type(self):
        return self._type
    type = property(_get_type)

    def _get_description(self):
        return self.daemon.describe(self.type)['description']
    description = property(_get_description)

    def _get_inputs(self):
        module = self.daemon.status['modules'][self.id]
        if module.has_key('inputs'):
            return module['inputs']
        else:
            return []
    inputs = property(_get_inputs)

    def _get_outputs(self):
        module = self.daemon.status['modules'][self.id]
        if module.has_key('outputs'):
            return module['outputs']
        else:
            return []
    outputs = property(_get_outputs)

    def get_property(self, property_name):
        return self.daemon.get_module_property(self.id, property_name)

    def set_property(self, property_name, val):
        self.daemon.set_module_property(self.id, property_name, val)

    def __repr__(self):
        return "<"+self.type + "Module  properties=" + str(self.properties)+">"


class MovidDaemon(object):

    def __init__(self, daemon="http://127.0.0.1:7500"):
        self.daemon = daemon
        self.update_status()


    def update_status(self):
        self.modules = {}
        for id, module in self.status['modules'].items():
            print 'reading mocdules:', id, module['name']
            self.modules[id] = otModuleProxy(self, id, module['name'])


    def remote_call(self, address, selection=None):
        url = self.daemon + address
        #print "calling: ", url
        connection = urllib.urlopen(url)
        response = connection.read()
        connection.close()
        result =  json.loads( response )

        if result['success'] == 0:
            raise Exception("OpenTracker Error: \n Message: %s"% result['message'])

        if selection:
            return result[selection]
        return result


    def _list_modules(self):
        return self.remote_call('/factory/list', 'list')
    available_modules = property(_list_modules)

    def _get_status(self):
        return self.remote_call('/pipeline/status', 'status')
    status = property(_get_status)

    def is_running(self):
        return self.status['running'] == 1

    def describe(self, name):
        return self.remote_call('/factory/describe?name='+name, 'describe')

    def start(self):
        self.remote_call('/pipeline/start')

    def stop(self):
        self.remote_call('/pipeline/stop')

    def quit(self):
        self.remote_call('/pipeline/quit')

    def rename_module(self, module_id, new_id):
        #print "renaming ", new_id
        if not self.modules.has_key(module_id):
            raise Excpetion("Can't rename!  No module named: %s"%module_id)
        module = self.modules[module_id]
        module._id = new_id
        del self.modules[module_id]
        self.modules[new_id] = module


    def add_module(self, objectname, id=None):
        new_id = self.remote_call('/pipeline/create?objectname='+objectname, 'message')
        #print "Created:", new_id, "    from:", objectname
        module = otModuleProxy(self, new_id, objectname)
        self.modules[module.id] = module
        if id:
            self.modules[new_id].id = id
            new_id = id
        return self.modules[new_id]

    def get_module_property(self, module_id, property_name):
        url = '/pipeline/get?objectname=%s&name=%s' % (module_id, property_name)
        return self.remote_call(url)

    def set_module_property(self, module_id, property_name, value):
        #print "setting" , module_id, property_name, value
        url = '/pipeline/set?objectname=%s&name=%s&value=%s' % (module_id, property_name, str(value))
        return self.remote_call(url)

    def connect_module_by_index(self, out_module, out_port, in_module, in_port):
        url = '/pipeline/connect?in=%s&out=%s&inidx=%s&outidx=%s' % (in_module,out_module, str(in_port), str(out_port))
        #print url
        self.remote_call(url)
