#pragma once

#include <cstdint>

constexpr static uint16_t IOPortConfig = 0x00;
constexpr static uint16_t IOPortData = 0x01;
constexpr static uint16_t StackPointerBase = 0x100;
constexpr static uint16_t VectorNMI = 0xfffa;
constexpr static uint16_t VectorRESET = 0xfffc;
constexpr static uint16_t VectorIRQ = 0xfffe;
