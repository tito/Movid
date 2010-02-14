#ifndef OT_MODULE_H
#define OT_MODULE_H

class otDataStream;

class otModule {
	
public:	
	virtual ~otModule();
	
	virtual void update();
	virtual void setInput( otDataStream* , int n=0) = 0;	
	virtual otDataStream* getOutput( int ) = 0;
	
};

#endif