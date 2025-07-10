#include <ui.h>
#include <display.h>




void rectangle_widget(rectangle_widget_t *rectangle_widget_config)
{
    v_display_draw_round_rectangle(rectangle_widget_config->x,
        rectangle_widget_config->y,
        rectangle_widget_config->width,
        rectangle_widget_config->height,
        rectangle_widget_config->radius,
        rectangle_widget_config->color
    );

    v_draw_text_gfx(
        rectangle_widget_config->x + rectangle_widget_config->padding_x,
        rectangle_widget_config->y + rectangle_widget_config->height - rectangle_widget_config->padding_y,
        rectangle_widget_config->text,
        rectangle_widget_config->font,
        rectangle_widget_config->text_color
    );
}

void arc_widget(arc_widget_t *arc_widget_config)
{
    // Draw the arc
    v_display_draw_arc(
        arc_widget_config->x, arc_widget_config->y,
        arc_widget_config->radius, arc_widget_config->thickness,
        arc_widget_config->start_angle, arc_widget_config->end_angle,
        arc_widget_config->color
    );

    // Calculate string width in pixels
    int text_width = 0;
    const char *str = arc_widget_config->text;
    while (*str) {
        char c = *str++;
        if (c < arc_widget_config->font->first || c > arc_widget_config->font->last) continue;
        const GFXglyph *glyph = &arc_widget_config->font->glyph[c - arc_widget_config->font->first];
        text_width += glyph->xAdvance;
    }

    // Calculate approximate text height using yAdvance
    int text_height = arc_widget_config->font->yAdvance;

    // Calculate top-left coordinates to center text
    int16_t text_x = arc_widget_config->x + arc_widget_config->padding_x + arc_widget_config->radius - (text_width);
    int16_t text_y = arc_widget_config->y + arc_widget_config->padding_y + arc_widget_config->radius - (text_height);

    // Draw the centered text
    v_draw_text_gfx_rotated(
        text_x,
        text_y,
        arc_widget_config->text,
        arc_widget_config->font,
        arc_widget_config->text_color,
        arc_widget_config->text_angle  // Set angle to 0° for flat horizontal text in center
    );
}