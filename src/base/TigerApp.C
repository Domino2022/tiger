#include "TigerApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"


InputParameters
TigerApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  return params;
}

TigerApp::TigerApp(InputParameters parameters) : MooseApp(parameters)
{
  TigerApp::registerAll(_factory, _action_factory, _syntax);
}

TigerApp::~TigerApp() {}

void
TigerApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"TigerApp"});
  Registry::registerActionsTo(af, {"TigerApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
TigerApp::registerApps()
{
  registerApp(TigerApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
TigerApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  TigerApp::registerAll(f, af, s);
}
extern "C" void
TigerApp__registerApps()
{
  TigerApp::registerApps();
}
