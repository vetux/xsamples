/**
 *  Mana - 3D Game Engine
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

#include "platform/graphics/rendercommand.hpp"

std::string formatBlendMode(engine::BlendMode mode) {
    switch (mode) {
        case engine::ZERO:
            return "ZERO";
        case engine::ONE:
            return "ONE";
        case engine::SRC_COLOR:
            return "SRC_COLOR";
        case engine::ONE_MINUS_SRC_COLOR:
            return "ONE_MINUS_SRC_COLOR";
        case engine::DST_COLOR:
            return "DST_COLOR";
        case engine::SRC_ALPHA:
            return "SRC_ALPHA";
        case engine::ONE_MINUS_SRC_ALPHA:
            return "ONE_MINUS_SRC_ALPHA";
        case engine::DST_ALPHA:
            return "DST_ALPHA";
        case engine::ONE_MINUS_DST_ALPHA:
            return "ONE_MINUS_DST_ALPHA";
        case engine::CONSTANT_COLOR:
            return "CONSTANT_COLOR";
        case engine::ONE_MINUS_CONSTANT_COLOR:
            return "ONE_MINUS_CONSTANT_COLOR";
        case engine::CONSTANT_ALPHA:
            return "CONSTANT_ALPHA";
        case engine::ONE_MINUS_CONSTANT_ALPHA:
            return "ONE_MINUS_CONSTANT_ALPHA";
        default:
            return "ERROR";
    }
}

std::string formatDepthTestMode(engine::DepthTestMode mode) {
    switch (mode) {
        case engine::DEPTH_TEST_ALWAYS:
            return "DEPTH_TEST_ALWAYS";
        case engine::DEPTH_TEST_NEVER:
            return "DEPTH_TEST_NEVER";
        case engine::DEPTH_TEST_LESS:
            return "DEPTH_TEST_LESS";
        case engine::DEPTH_TEST_EQUAL:
            return "DEPTH_TEST_EQUAL";
        case engine::DEPTH_TEST_LEQUAL:
            return "DEPTH_TEST_LEQUAL";
        case engine::DEPTH_TEST_GREATER:
            return "DEPTH_TEST_GREATER";
        case engine::DEPTH_TEST_NOTEQUAL:
            return "DEPTH_TEST_NOTEQUAL";
        case engine::DEPTH_TEST_GEQUAL:
            return "DEPTH_TEST_GEQUAL";
        default:
            return "ERROR";
    }
}

#endif //MANA_STRINGFORMAT_HPP
