/*
 * Copyright 2017 Seth Traverse
 *
 * This file is part of ViaWare Media Centre.
 *
 * ViaWare Media Centre is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ViaWare Media Centre is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * with ViaWare Media Centre.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef INPUTROUTES_H
#define INPUTROUTES_H

#include "RouteGroup.h"

#include <vmc-libselect/InputManager.h>

namespace vmc
{
    namespace routes
    {
        class InputRoutes : public RouteGroup
        {
        public:
            InputRoutes(Config const *config, input::InputManager *inputManager);

            void initRoutes(Router &router) override;
        private:
            void getInputs(RouterRequest &request);
            void setInput(RouterRequest &request);

            input::InputManager *inputManager;
        };
    }
}

#endif
