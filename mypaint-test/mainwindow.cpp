#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mypaint/mypaint-fixed-tiled-surface.h"
#include "mypaint/mypaint-brush.h"

#define CONV_16_8(x) ((x*255)/(1<<15))

void 
stroke_to(MyPaintBrush *brush, MyPaintSurface *surf, float x, float y, float viewzoom, float viewrotation)
{
    float pressure = 1.0, ytilt = 0.0, xtilt = 0.0, dtime = 1.0 / 10;
    mypaint_brush_stroke_to(brush, surf, x, y, pressure, xtilt, ytilt, dtime);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MyPaintBrush *brush = mypaint_brush_new();
    MyPaintFixedTiledSurface *surface = mypaint_fixed_tiled_surface_new(512, 512);

    mypaint_brush_from_defaults(brush);
    mypaint_brush_set_base_value(brush, MYPAINT_BRUSH_SETTING_COLOR_H, 0.0);
    mypaint_brush_set_base_value(brush, MYPAINT_BRUSH_SETTING_COLOR_S, 1.0);
    mypaint_brush_set_base_value(brush, MYPAINT_BRUSH_SETTING_COLOR_V, 1.0);

    /* Draw a rectangle on surface with brush */
    mypaint_surface_begin_atomic((MyPaintSurface *)surface);
    stroke_to(brush, (MyPaintSurface *)surface, 0.0,     0.0,  1.0, 0.0);
    stroke_to(brush, (MyPaintSurface *)surface, 200.0,   0.0,  1.0, 0.0);
    stroke_to(brush, (MyPaintSurface *)surface, 200.0, 200.0,  1.0, 0.0);
    stroke_to(brush, (MyPaintSurface *)surface, 0.0,   200.0,  1.0, 0.0);
    stroke_to(brush, (MyPaintSurface *)surface, 0.0,     0.0,  1.0, 0.0);
    MyPaintRectangle roi;
    mypaint_surface_end_atomic((MyPaintSurface *)surface, &roi);

    uint16_t* buffer = mypaint_fixed_tiled_surface_get_buffer(surface);
    QImage img(512, 512, QImage::Format_ARGB32_Premultiplied);
    //memcpy(img.bits(), buffer, 512 * 512 * 2 * 2);
    //QRgb* p = (QRgb*)img.bits();
    for (int i = 0; i < 512; ++i)
    {
        for (int j = 0 ; j < 512; ++j)
        {
            int offset = (i * 512 + j) * 4;

            uint8_t r = CONV_16_8(buffer[offset + 0]);
            uint8_t g = CONV_16_8(buffer[offset + 1]);
            uint8_t b = CONV_16_8(buffer[offset + 2]);
            uint8_t a = CONV_16_8(buffer[offset + 3]);

            QColor c(r, g, b, a);
            
            //p++; // next image pixel...
            img.setPixelColor(i, j, c);
        }
    }
    img.save("C:/temp/ok.png");

    //MyPaintSurface* s = mypaint_fixed_tiled_surface_interface(surface);
    //mypaint_surface_save_png(s, "C:/test/ok.png", 0, 0, 500, 500);
    
    mypaint_brush_unref(brush);
    mypaint_surface_unref((MyPaintSurface *)surface);
}

MainWindow::~MainWindow()
{
    delete ui;
}
