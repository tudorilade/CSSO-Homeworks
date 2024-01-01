#pragma once

#include "Command.h"
#include "DirFileHandler.h"
#include <vector>
#include <chrono>
#include <Aclapi.h>


typedef struct BitmapHeadersInfo {
	BITMAPFILEHEADER bfHeader;
	BITMAPINFOHEADER biHeader;
	string toString();
} bHeaders;

struct Pixel {
	uint8_t red, green, blue, alpha;
};

typedef struct CommandRequiredInfo
{
	DirFileHandler fileHandler;
	DWORD startingByteForPixels;
	wstring greyDirPath;
	wstring inverseDirPath;
	wstring imageName;
	wstring imagePath;
	bHeaders headerBitmap;

	void setInfo(
		DirFileHandler,
		wstring,
		wstring,
		wstring,
		wstring,
		bHeaders
	);

	void loadImage(vector<Pixel>&);
	BOOL writeImage(wstring, vector<Pixel>&);
	SECURITY_ATTRIBUTES* setupSecurityDescriptor();

} cmdInfo;


class GreyScale {
public:
	Pixel operation(Pixel&);
};

class GreyScaleCommand : public Command {
	GreyScale grey = GreyScale();
public:
	void execute(evPerfResults&) override {};
	void execute(myPerfResults&) override {};
	void execute(vector<Pixel>&, vector<Pixel>&);
	void execute(Pixel&, vector<Pixel>&);
	string getExecuteLog() override { return "Executing grey operation..."; };
};


class InverseScale {
public:
	Pixel operation(Pixel&);
};

class InverseScaleCommand : public Command {
	InverseScale inverse = InverseScale();
public:
	void execute(evPerfResults&) override {};
	void execute(myPerfResults&) override {};
	void execute(vector<Pixel>&, vector<Pixel>&);
	void execute(Pixel&, vector<Pixel>&);
	string getExecuteLog() override {return "Executing grey operation...";}
};


class Base {

protected:
	cmdInfo cInfo;
	InverseScaleCommand inverseCommand = InverseScaleCommand();
	GreyScaleCommand greyScaleCommand = GreyScaleCommand();
	vector<Pixel> imagePixels;

public:
	Base() {  };
	Base(cmdInfo);
	void loadImage();
	BOOL failedToLoadImage();
	BOOL writeGreyImage(vector<Pixel>&, wstring, evPerfResults&);
	BOOL writeInverseImage(vector<Pixel>&, wstring, evPerfResults&);
};


//
// Command for sequential
//

class Sequential : public Base {
public:
	Sequential() {};
	Sequential(cmdInfo);

	void executeInverse(vector<Pixel>&);
	void executeGrey(vector<Pixel>&);
};

class SequentialCommand : public Command {
	Sequential sequential;
public:

	SequentialCommand() : Command() {};
	SequentialCommand(cmdInfo);
	void execute(myPerfResults&) override {};
	void execute(evPerfResults&) override;
	string getExecuteLog() { return "Executing sequential command..."; };
};



//
// Command for dynamic 
//

class Dynamic : public Base {
public:
	Dynamic() {};
	Dynamic(cmdInfo);

	void executeInverse(vector<Pixel>&, vector<Pixel>&);
	void executeGrey(vector<Pixel>&, vector<Pixel>&);
};

class DynamicCommand : public Command {
	Dynamic dynamic;
public:

	DynamicCommand() : Command() {};
	DynamicCommand(cmdInfo);
	void execute(myPerfResults&) override {};
	void execute(evPerfResults&) override;
	string getExecuteLog() { return "Executing dynamic command..."; };
};

//
// Command for static
//

class Static {
private:
	string imagePath;
	DirFileHandler fHandler;

public:
	Static() {};
	Static(string, DirFileHandler);
};

class StaticCommand : public Command {
	Static sTatic = Static();
public:
	void execute(myPerfResults&) override {};
	void execute(evPerfResults&) override;
};

