using DrawCallback = bool(IDirect3DDevice9* device);

void InitializeImGui(DrawCallback* cb);
static void DrawInternal(IDirect3DDevice9* device);
bool IsRunning();