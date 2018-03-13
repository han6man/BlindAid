#pragma once

#include <iostream>
#include <conio.h>

#include "Config.h"
#include "..\Modules\GlobalParameters.h"
#include "..\Modules\Core.h"

class Menu
{
public:
  Menu(Core::Core *core, Core::Parameters *params, Core::Data *data, Logger *logger)
  {
    _core = core;
    _params = params;
    _results = data;
    _logger = logger;
  }

  virtual void ShowMenu() = 0;

protected:
  void Process()
  {
    if (_core == NULL)
      _core = new Core::Core(_params, NULL, _results, _logger);

    (*_core)();
  }

  Core::Core *_core;
  Core::Parameters *_params;
  Core::Data *_results;
  Logger *_logger;
};