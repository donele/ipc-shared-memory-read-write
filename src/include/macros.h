#pragma once

#define NO_COPY(T) T(const T&) = delete; T& operator=(const T&) = delete
#define NO_MOVE(T) T(T&&) = delete; T& operator=(T&&) = delete
