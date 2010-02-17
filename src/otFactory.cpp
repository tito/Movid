#include <string>

#include "otLog.h"
#include "otFactory.h"

#define REGISTER_MODULE(name) \
	extern otModule* factory_create_ot##name(); \
	LOG(DEBUG) << "register <" #name "> module"; \
	otFactory::getInstance()->registerModule(#name, factory_create_ot##name);

LOG_DECLARE("Factory");

static otFactory *instance = NULL;

void otFactory::init() {
	REGISTER_MODULE(Amplify);
	REGISTER_MODULE(BackgroundSubtract);
	REGISTER_MODULE(Camera);
	REGISTER_MODULE(GrayScale);
	REGISTER_MODULE(Highpass);
	REGISTER_MODULE(ImageDisplay);
	REGISTER_MODULE(Invert);
	REGISTER_MODULE(Smooth);
	REGISTER_MODULE(Threshold);
	REGISTER_MODULE(Video);
	LOG(INFO) << "register " << otFactory::getInstance()->list().size() << " modules";
}

otFactory::otFactory() {
}

otFactory::~otFactory() {
}

otFactory *otFactory::getInstance() {
	if ( instance == NULL )
		instance = new otFactory();
	return instance;
}

void otFactory::registerModule(const std::string &name, otFactoryCreateCallback callback) {
	this->database[name] = callback;
}

otModule *otFactory::create(const std::string &name) {
	std::string sname = std::string(name);

	// for easier creation, remove the ot at start
	if ( sname.size() > 2 && sname.substr(0, 2) == "ot" )
		sname =	sname.substr(2, sname.length() - 2);
	if ( sname.size() > 6 && sname.substr(sname.length() - 6, 6) == "Module" )
		sname = sname.substr(0, sname.length() - 6);

	std::map<std::string, otFactoryCreateCallback>::iterator it;
	it = this->database.find(sname);
	if ( it == this->database.end() )
		return NULL;
	return (it->second)();
}

std::vector<std::string> otFactory::list() {
	std::vector<std::string> l;
	std::map<std::string, otFactoryCreateCallback>::iterator it;
	for ( it = this->database.begin(); it != this->database.end(); it++ )
		l.push_back(it->first);
	return l;
}
