#pragma once

#include "stdafx.h"
#include "PowerPC Assembly Functions.h"
#include "Code Menu.h"

#define IASA_BUFFER_LENGTH 20
#define IASA_BUFFER_SIZE IASA_BUFFER_LENGTH * 8
#define IASA_TRIGGER_BUFFER_SIZE 5 * 8
#define IASA_TERMINATE_BUFFER_SIZE 1 * 8

void EndOverlay();

void TriggerIASAOverlay();

void SetupIASABuffers();

void FreeIASABuffers();
