unsigned doorwayCoords(double x, double y) {
    static unsigned color = (RAND_COLOR() & 0x00FFFFFF) | TRAVERSAL_ALPHA;
    if(x == 0 && y == 0) color = (RAND_COLOR() & 0x00FFFFFF) | TRAVERSAL_ALPHA; // new image has started

    if (x > 0.25 && x < 0.75 && y > 0.25 && y < 0.75) return NO_COLOR;
    else return blend_colors(color, NO_COLOR, sqrt(pow(0.5 - x, 2.0) + pow(0.5 - y, 2.0)));
}

unsigned radialCoords(double x, double y) {
    static unsigned color = (RAND_COLOR() & 0x00FFFFFF) | TRAVERSAL_ALPHA;
    if(x == 0 && y == 0) color = (RAND_COLOR() & 0x00FFFFFF) | TRAVERSAL_ALPHA; // new image has started

    if (sqrt(pow(0.5 - x, 2.0) + pow(0.5 - y, 2.0)) < 0.5F) return NO_COLOR;
    else return blend_colors(color, NO_COLOR, sqrt(pow(0.5 - x, 2.0) + pow(0.5 - y, 2.0)));
}