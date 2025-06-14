#include "common.h"

int main(int argc, char** argv) {
    simple_wav_t float_form = read_simple_wav(stdin);


    int _width = 500, _height = 500;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(_width, _height, "graph display");
    SetTargetFPS(30);

    float* combined_frequencies = compute_complex_absolute_values(float_form.samples, float_form.nr_sample_points/2);
    float* normalized_data = normalize_float_array(combined_frequencies, float_form.nr_sample_points/2);
    Vector2* normalized_graph = create_graph_from_float_array(normalized_data, float_form.nr_sample_points/2, 15.0f, 350.0f, 350.0f, 200.0f);

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawSplineLinear(normalized_graph, float_form.nr_sample_points/2, 1.0f, BLUE);

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
