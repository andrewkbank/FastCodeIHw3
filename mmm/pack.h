//Honestly, I probably didn't need to use an LLM for this, but I'm a busy guy what can I say
//One shot with GPT-5.6 Sol using the prompt in
void pack(double* pack_a, double* a, int mc, int m, int k)
{
    int num_panels = mc / m;

    for (int p = 0; p < num_panels; ++p) {

        double *panel_src  = a + p * m * k;
        double *panel_dest = pack_a + p * m * k;

        for (int col = 0; col < k; ++col) {
            for (int row = 0; row < m; ++row) {

                panel_dest[col*m + row] =
                    panel_src[row*k + col];
            }
        }
    }
}