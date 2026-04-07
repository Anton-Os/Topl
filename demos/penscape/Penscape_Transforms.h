Vec3f getStepTransform(float amount){
    static unsigned invocation = 0;

    switch (invocation % 6) {
        case 0: return Vec3f({ amount, 0.0F, 0.0F });
        case 1: return Vec3f({ -amount, 0.0F, 0.0F });
        case 2: return Vec3f({ 0.0F, amount, 0.0F });
        case 3: return Vec3f({ 0.0F, -amount, 0.0F });
        case 4: return Vec3f({ 0.0F, 0.0F, amount });
        case 5: return Vec3f({ 0.0F, 0.0F, -amount });
        default: return Vec3f({ -amount, -amount, -amount });
    }

    invocation++;
}