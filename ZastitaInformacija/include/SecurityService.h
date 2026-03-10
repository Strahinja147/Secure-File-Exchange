#pragma once
#include <string>
#include <iostream>
#include "FileManager.h"
#include "A52.h"
#include "SimpleSubstitution.h"
#include "MD5.h"
#include "MetadataManager.h"
#include "Logger.h"
#include "KeyHelper.h"
using namespace std;
class SecurityService
{
public:
	SecurityService();
	bool ZastitiFajl(const string& imeFajla, int algoritamZaKodiranje, string lozinka);
	bool OdpakujFajl(const string& imeFajla, string lozinka);
};

