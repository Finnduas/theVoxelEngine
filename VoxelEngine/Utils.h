#pragma once
#include <stdio.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "include/glad/glad.h"
#include "Constants.h"

void get_openGL_version_info() {
	printf("Vendor: "); printf(glGetString(GL_VENDOR)); printf("\n");
	printf("Version: "); printf(glGetString(GL_VERSION)); printf("\n");
	printf("Shading language: "); printf(glGetString(GL_SHADING_LANGUAGE_VERSION)); printf("\n");
}