@echo off
@echo Building application
clang src/*.c lib/glad/src/glad.c -o bin/natsim.exe  -I includes/nat-sim -I includes -I includes/GLFW -Llib/GLFW/lib-vc2022 -lglfw3dll -lgdi32 -lopengl32
IF %ERRORLEVEL% NEQ 0 (
    echo Build Failed
    exit /b %errorlevel%
) ELSE (
    echo Build Successful
)