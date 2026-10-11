Vec3f getStepTransform(float amount){
    static unsigned invocation = 0;
    invocation++;

    switch (invocation % 6) {
        case 0: return Vec3f({ amount, 0.0F, 0.0F });
        case 1: return Vec3f({ -amount, 0.0F, 0.0F });
        case 2: return Vec3f({ 0.0F, amount, 0.0F });
        case 3: return Vec3f({ 0.0F, -amount, 0.0F });
        case 4: return Vec3f({ 0.0F, 0.0F, amount });
        case 5: return Vec3f({ 0.0F, 0.0F, -amount });
        default: return Vec3f({ -amount, -amount, -amount });
    }
}

Vec3f getTurnTransform(float amount){
    static unsigned invocation = 0;
    invocation++;

    switch (invocation % 6) {
        case 0: return Vec3f({ sinf(amount * (invocation % 30)), 0.0F, 0.0F });
        case 1: return Vec3f({ cosf(amount * (invocation % 30)), 0.0F, 0.0F });
        case 2: return Vec3f({ 0.0F, sinf(amount * (invocation % 30)), 0.0F });
        case 3: return Vec3f({ 0.0F, -cosf(amount * (invocation % 30)), 0.0F });
        case 4: return Vec3f({ 0.0F, 0.0F, -sinf(amount * (invocation % 30)) });
        case 5: return Vec3f({ 0.0F, 0.0F, -cosf(amount * (invocation % 30)) });
        default: return Vec3f({ tan(amount), tan(amount), tan(amount), });
    }
}