#include "rt.h"
#include <font.h>
#include <stdint.h>
#include <mm/memop.h>

static rt_context _curctx;

void _rt_drawchar(unsigned char c, int x, int y, int fgcolor, int bgcolor)
{
	int cx,cy;
	int mask[8]={128, 64, 32, 16, 8, 4, 2, 1};
	unsigned char *glyph=VGA8+(int)c*16;

    uint32_t *buf = _curctx.framebuffer;

	for(cy=0;cy<16;cy++){
		for(cx=0;cx<8;cx++){
            buf[((y + cy) * _curctx.framebuffer_width) + (x + cx)] = glyph[cy]&mask[cx]?fgcolor:bgcolor;
		}
	}
}
void _rt_draw_fillchar(int x, int y, int bgcolor, int fgcolor) {
	int cx,cy;
    uint32_t *buf = _curctx.framebuffer;
	for(cy=0;cy<16;cy++){
		for(cx=0;cx<8;cx++){
            buf[((y + cy) * _curctx.framebuffer_width) + (x + cx)] = cy > 12 ? fgcolor : bgcolor;
		}
	}
}

int _rt_strlen(char *str) {
    int i = 0;
    while (str[i] != '\0') i++;
    return i;
}

void rt_init(rt_context ctx) {
    // Copy the structure
    char *src = (char*)&ctx;
    char *dst = (char*)&_curctx;
    for (unsigned long i = 0; i < sizeof(rt_context); i++) {
        dst[i] = src[i];
    }

    // Fill fields
    _curctx.term_width = _curctx.framebuffer_width / 8;
    _curctx.term_height = _curctx.framebuffer_height / 16;
}

void rt_print(char *str) {
    _rt_draw_fillchar(_curctx.x * 8, _curctx.y * 16, 0x0, 0x0);

    for (int i = 0; i < _rt_strlen(str); i++) {
        if (str[i] == '\n' && _curctx.use_crlf_ending)
            if (_curctx.y * 16 >= _curctx.framebuffer_height) {
                _curctx.y = 0;
                memset(
                    _curctx.framebuffer, _curctx.bg_color,
                    _curctx.framebuffer_width * _curctx.framebuffer_height * sizeof(uint32_t));
            } else {
                _curctx.y++;
            }
        else if (str[i] == '\n')
        {
            if (_curctx.y * 16 >= _curctx.framebuffer_height) {
                _curctx.y = 0;
                memset(
                    _curctx.framebuffer, _curctx.bg_color,
                    _curctx.framebuffer_width * _curctx.framebuffer_height * sizeof(uint32_t));
            } else {
                _curctx.y++;
            }
            _curctx.x = 0;
        }
        else if (str[i] == '\r')
            _curctx.x = 0;
        else {
            _rt_drawchar(str[i], _curctx.x * 8, _curctx.y * 16, 0xFFFFFF, 0x0);
            _curctx.x++;
        }
    }

    _rt_draw_fillchar(_curctx.x * 8, _curctx.y * 16, 0x0, 0xFFFFFF);
}