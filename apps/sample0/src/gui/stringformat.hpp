/**
 *  xEngine-Samples - Example applications demonstrating the xEngine
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MANA_STRINGFORMAT_HPP
#define MANA_STRINGFORMAT_HPP

#include <string>

#include "render/platform/rendercommand.hpp"

std::string formatBlendMode(xengine::BlendMode mode) {
    switch (mode) {
        case xengine::ZERO:
            return "ZERO";
        case xengine::ONE:
            return "ONE";
        case xengine::SRC_COLOR:
            return "SRC_COLOR";
        case xengine::ONE_MINUS_SRC_COLOR:
            return "ONE_MINUS_SRC_COLOR";
        case xengine::DST_COLOR:
            return "DST_COLOR";
        case xengine::SRC_ALPHA:
            return "SRC_ALPHA";
        case xengine::ONE_MINUS_SRC_ALPHA:
            return "ONE_MINUS_SRC_ALPHA";
        case xengine::DST_ALPHA:
            return "DST_ALPHA";
        case xengine::ONE_MINUS_DST_ALPHA:
            return "ONE_MINUS_DST_ALPHA";
        case xengine::CONSTANT_COLOR:
            return "CONSTANT_COLOR";
        case xengine::ONE_MINUS_CONSTANT_COLOR:
            return "ONE_MINUS_CONSTANT_COLOR";
        case xengine::CONSTANT_ALPHA:
            return "CONSTANT_ALPHA";
        case xengine::ONE_MINUS_CONSTANT_ALPHA:
            return "ONE_MINUS_CONSTANT_ALPHA";
        default:
            return "ERROR";
    }
}

std::string formatDepthTestMode(xengine::DepthTestMode mode) {
    switch (mode) {
        case xengine::DEPTH_TEST_ALWAYS:
            return "DEPTH_TEST_ALWAYS";
        case xengine::DEPTH_TEST_NEVER:
            return "DEPTH_TEST_NEVER";
        case xengine::DEPTH_TEST_LESS:
            return "DEPTH_TEST_LESS";
        case xengine::DEPTH_TEST_EQUAL:
            return "DEPTH_TEST_EQUAL";
        case xengine::DEPTH_TEST_LEQUAL:
            return "DEPTH_TEST_LEQUAL";
        case xengine::DEPTH_TEST_GREATER:
            return "DEPTH_TEST_GREATER";
        case xengine::DEPTH_TEST_NOTEQUAL:
            return "DEPTH_TEST_NOTEQUAL";
        case xengine::DEPTH_TEST_GEQUAL:
            return "DEPTH_TEST_GEQUAL";
        default:
            return "ERROR";
    }
}

#endif //MANA_STRINGFORMAT_HPP
