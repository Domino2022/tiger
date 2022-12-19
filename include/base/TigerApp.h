#ifndef TIGERAPP_H
#define TIGERAPP_H

#include "MooseApp.h"


class TigerApp : public MooseApp
{
public:
  static InputParameters validParams();
  TigerApp(InputParameters parameters);
  virtual ~TigerApp();

  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s);
};

#endif /* TIGERAPP_H */
