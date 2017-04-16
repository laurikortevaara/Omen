#include <iostream>

#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#include "Script.h"

using namespace omen;
using namespace ecs;

Script::Script(const char* scriptFile)
{
	try {
		// point to the relevant directories of the Mono installation
		mono_set_dirs("./mono/lib", "./mono/etc");

		// load the default Mono configuration file in 'etc/mono/config'
		mono_config_parse(NULL);

		MonoDomain* monoDomain = mono_jit_init_version("embedding_mono_domain", "v4.0.30319");
		MonoAssembly* monoAssembly = mono_domain_assembly_open(monoDomain, "OmenScriptCompiler");
		MonoImage* monoImage = mono_assembly_get_image(monoAssembly);
	}
	catch (...)
	{
		std::cout << "Caught an exepction..." << '\n';
	}

}

void Script::update(double t, double dt)
{

}