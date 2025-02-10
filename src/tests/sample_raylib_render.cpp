#include <memory>
#include <raylib.h>

// Todo: Consider mapload as first argument
int main(void) {
    const int screenWidth = 640;
    const int screenHeight = 480;
    // the holy resolution

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Test");
    SetTargetFPS(60);

    // Set up 3D mode

    std::unique_ptr<Camera3D> c3d = std::make_unique<Camera3D>();
    c3d->position = (Vector3){10.0f,10.0f,10.0f};
    c3d->up = (Vector3){0.0f, 1.0f, 0.0f};
    c3d->projection = CAMERA_PERSPECTIVE;
    c3d->fovy = 45.0f;
    c3d->target = (Vector3){0.0f, 0.0f, 0.0f};

    DisableCursor();

    while (!WindowShouldClose()) {
        UpdateCamera(c3d.get(), CAMERA_FREE);
        BeginDrawing();

            ClearBackground(WHITE);
            BeginMode3D(*c3d.get());
                DrawCube((Vector3){0.0f,0.0f,0.0f}, 2.0f, 2.0f, 2.0f, RED);
                DrawCubeWires((Vector3){0.0f,0.0f,0.0f}, 2.0f, 2.0f, 2.0f, RED);
                DrawGrid(10, 1.0f);
            EndMode3D();
            DrawText("TEST", 0, 0, 10, BLUE);


        EndDrawing();
    }

    CloseWindow();
    return 0;
}
