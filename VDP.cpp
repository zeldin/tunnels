#include "system.h"

#include "VDP.h"

namespace Tunnels { namespace VDP {

void BytemapRenderer::drawPattern(unsigned row, unsigned col,
				  const byte *pat, byte colors)
{
  if (row >= ROWS || col >= COLUMNS || !data)
    return;
  byte *pix = data + (row << 3)*pitch + (col << 3);
  byte fg = colors >> 4;
  byte bg = colors & 15;
  for (unsigned i=0; i<8; i++) {
    byte p = *pat++;
    for (unsigned j=0; j<8; j++, p <<= 1) 
      pix[j] = ((p & 0x80)? fg : bg);
    pix += pitch;
  }
}

void Screen::loadRomFont() {
  static constexpr byte rom_font[][7] {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /*   */
    { 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x10 }, /* ! */
    { 0x28, 0x28, 0x28, 0x00, 0x00, 0x00, 0x00 }, /* " */
    { 0x28, 0x28, 0x7c, 0x28, 0x7c, 0x28, 0x28 }, /* # */
    { 0x38, 0x54, 0x50, 0x38, 0x14, 0x54, 0x38 }, /* $ */
    { 0x60, 0x64, 0x08, 0x10, 0x20, 0x4c, 0x0c }, /* % */
    { 0x20, 0x50, 0x50, 0x20, 0x54, 0x48, 0x34 }, /* & */
    { 0x08, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00 }, /* ' */
    { 0x08, 0x10, 0x20, 0x20, 0x20, 0x10, 0x08 }, /* ( */
    { 0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20 }, /* ) */
    { 0x00, 0x28, 0x10, 0x7c, 0x10, 0x28, 0x00 }, /* * */
    { 0x00, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x00 }, /* + */
    { 0x00, 0x00, 0x00, 0x00, 0x30, 0x10, 0x20 }, /* , */
    { 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00 }, /* - */
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30 }, /* . */
    { 0x00, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00 }, /* / */
    { 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38 }, /* 0 */
    { 0x10, 0x30, 0x10, 0x10, 0x10, 0x10, 0x38 }, /* 1 */
    { 0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x7c }, /* 2 */
    { 0x38, 0x44, 0x04, 0x18, 0x04, 0x44, 0x38 }, /* 3 */
    { 0x08, 0x18, 0x28, 0x48, 0x7c, 0x08, 0x08 }, /* 4 */
    { 0x7c, 0x40, 0x78, 0x04, 0x04, 0x44, 0x38 }, /* 5 */
    { 0x18, 0x20, 0x40, 0x78, 0x44, 0x44, 0x38 }, /* 6 */
    { 0x7c, 0x04, 0x08, 0x10, 0x20, 0x20, 0x20 }, /* 7 */
    { 0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38 }, /* 8 */
    { 0x38, 0x44, 0x44, 0x3c, 0x04, 0x08, 0x30 }, /* 9 */
    { 0x00, 0x30, 0x30, 0x00, 0x30, 0x30, 0x00 }, /* : */
    { 0x00, 0x30, 0x30, 0x00, 0x30, 0x10, 0x20 }, /* ; */
    { 0x08, 0x10, 0x20, 0x40, 0x20, 0x10, 0x08 }, /* < */
    { 0x00, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x00 }, /* = */
    { 0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20 }, /* > */
    { 0x38, 0x44, 0x04, 0x08, 0x10, 0x00, 0x10 }, /* ? */
    { 0x38, 0x44, 0x5c, 0x54, 0x5c, 0x40, 0x38 }, /* @ */
    { 0x38, 0x44, 0x44, 0x7c, 0x44, 0x44, 0x44 }, /* A */
    { 0x78, 0x24, 0x24, 0x38, 0x24, 0x24, 0x78 }, /* B */
    { 0x38, 0x44, 0x40, 0x40, 0x40, 0x44, 0x38 }, /* C */
    { 0x78, 0x24, 0x24, 0x24, 0x24, 0x24, 0x78 }, /* D */
    { 0x7c, 0x40, 0x40, 0x78, 0x40, 0x40, 0x7c }, /* E */
    { 0x7c, 0x40, 0x40, 0x78, 0x40, 0x40, 0x40 }, /* F */
    { 0x3c, 0x40, 0x40, 0x5c, 0x44, 0x44, 0x38 }, /* G */
    { 0x44, 0x44, 0x44, 0x7c, 0x44, 0x44, 0x44 }, /* H */
    { 0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38 }, /* I */
    { 0x04, 0x04, 0x04, 0x04, 0x04, 0x44, 0x38 }, /* J */
    { 0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44 }, /* K */
    { 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7c }, /* L */
    { 0x44, 0x6c, 0x54, 0x54, 0x44, 0x44, 0x44 }, /* M */
    { 0x44, 0x64, 0x64, 0x54, 0x4c, 0x4c, 0x44 }, /* N */
    { 0x7c, 0x44, 0x44, 0x44, 0x44, 0x44, 0x7c }, /* O */
    { 0x78, 0x44, 0x44, 0x78, 0x40, 0x40, 0x40 }, /* P */
    { 0x38, 0x44, 0x44, 0x44, 0x54, 0x48, 0x34 }, /* Q */
    { 0x78, 0x44, 0x44, 0x78, 0x50, 0x48, 0x44 }, /* R */
    { 0x38, 0x44, 0x40, 0x38, 0x04, 0x44, 0x38 }, /* S */
    { 0x7c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 }, /* T */
    { 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38 }, /* U */
    { 0x44, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10 }, /* V */
    { 0x44, 0x44, 0x44, 0x54, 0x54, 0x54, 0x28 }, /* W */
    { 0x44, 0x44, 0x28, 0x10, 0x28, 0x44, 0x44 }, /* X */
    { 0x44, 0x44, 0x28, 0x10, 0x10, 0x10, 0x10 }, /* Y */
    { 0x7c, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7c }, /* Z */
    { 0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38 }, /* [ */
    { 0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00 }, /* \ */
    { 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38 }, /* ] */
    { 0x00, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00 }, /* ^ */
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c }, /* _ */
  };
  loadPatterns(32, rom_font);
}

void Screen::loadColorTable(unsigned idx, const byte *p, unsigned n)
{
  if (idx >= COLOR_TABLE_ENTRIES || !n)
    return;
  if (n > COLOR_TABLE_ENTRIES - idx)
    n = COLOR_TABLE_ENTRIES - idx;
  byte *t = &color_table[idx];
  bool *d = &pattern_generator_dirty[idx << 3];
  while (n--) {
    *t++ = *p++;
    *d++ = true;
    *d++ = true;
    *d++ = true;
    *d++ = true;
    *d++ = true;
    *d++ = true;
    *d++ = true;
    *d++ = true;
  }
  any_pattern_generator_dirty = true;
}

void Screen::loadPatterns7(unsigned idx, const byte *p, unsigned n)
{
  if (idx >= PATTERNS || !n)
    return;
  if (n > PATTERNS - idx)
    n = PATTERNS - idx;
  byte *g = pattern_generator[idx];
  bool *d = &pattern_generator_dirty[idx];
  while (n--) {
    *g++ = 0;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *d++ = true;
  }
  any_pattern_generator_dirty = true;
}

void Screen::loadPatterns8(unsigned idx, const byte *p, unsigned n)
{
  if (idx >= PATTERNS || !n)
    return;
  if (n > PATTERNS - idx)
    n = PATTERNS - idx;
  byte *g = pattern_generator[idx];
  bool *d = &pattern_generator_dirty[idx];
  while (n--) {
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *g++ = *p++;
    *d++ = true;
  }
  any_pattern_generator_dirty = true;
}

void Screen::hchar(unsigned row, unsigned col, byte name, unsigned cnt)
{
  if (row >= ROWS || col >= COLUMNS || !cnt)
    return;
  while (cnt > COLUMNS-col) {
    cnt -= COLUMNS-col;
    hchar(row, col, name, COLUMNS-col);
    col = 0;
    if (++row >= ROWS)
      return;
  }
  byte *n = &name_table[row][col];
  bool *d = &name_table_dirty[row][col];
  while(cnt--) {
    *d++ = (*n != name);
    *n++ = name;
  }
  any_name_table_dirty = true;
}

void Screen::vchar(unsigned row, unsigned col, byte name, unsigned cnt)
{
  if (row >= ROWS || col >= COLUMNS || !cnt)
    return;
  while (cnt > ROWS-row) {
    cnt -= ROWS-row;
    vchar(row, col, name, ROWS-row);
    row = 0;
    if (++col >= COLUMNS)
      return;
  }
  byte *n = &name_table[row][col];
  bool *d = &name_table_dirty[row][col];
  while(cnt--) {
    *d = (*n != name);
    *n = name;
    d += COLUMNS;
    n += COLUMNS;
  }
  any_name_table_dirty = true;
}

void Screen::hstr(unsigned row, unsigned col, const byte *str, unsigned len,
		  byte offset)
{
  if (row >= ROWS || col >= COLUMNS)
    return;
  while (len--) {
    hchar(row, col++, offset+*str++, 1);
    if (col >= COLUMNS) {
      col = 0;
      if (++row >= ROWS)
	return;
    }
  }
}

void Screen::vstr(unsigned row, unsigned col, const byte *str, unsigned len,
		  byte offset)
{
  if (row >= ROWS || col >= COLUMNS)
    return;
  while (len--) {
    vchar(row++, col, offset+*str++, 1);
    if (row >= ROWS) {
      row = 0;
      if (++col >= COLUMNS)
	return;
    }
  }
}

void Screen::hstr(unsigned row, unsigned col, const char *str)
{
  hstr(row, col, reinterpret_cast<const byte *>(str), strlen(str));
}
  
void Screen::vstr(unsigned row, unsigned col, const char *str)
{
  vstr(row, col, reinterpret_cast<const byte *>(str), strlen(str));
}
  
void Screen::all(byte name)
{
  hchar(0, 0, name, ROWS*COLUMNS);
}

void Screen::refresh(Backend &backend)
{
  if (!any_name_table_dirty && !any_pattern_generator_dirty)
    return;
  unsigned i, j;
  for (i=0; i<ROWS; i++) {
    for (j=0; j<COLUMNS; j++)
      if (name_table_dirty[i][j] ||
	  pattern_generator_dirty[name_table[i][j]])
	break;
    if (j<COLUMNS)
      break;
  }
  if (i<ROWS) {
    ScopedRender r{backend.startRender()};
    byte name;
    for (; i<ROWS; i++)
      for (j=0; j<COLUMNS; j++)
	if (name_table_dirty[i][j] ||
	    pattern_generator_dirty[name_table[i][j]]) {	
	  name = name_table[i][j];
	  r.drawPattern(i, j, pattern_generator[name], color_table[name>>3]);
	  name_table_dirty[i][j] = false;
	}
  }
  any_name_table_dirty = false;
  if (any_pattern_generator_dirty) {
    memset(pattern_generator_dirty, 0, sizeof(pattern_generator_dirty));
    any_pattern_generator_dirty = false;
  }
}

Screen::Screen()
{
  memset(pattern_generator, 0, sizeof(pattern_generator));
  memset(color_table, 0, sizeof(color_table));
  memset(name_table, 0, sizeof(name_table));
  memset(pattern_generator_dirty, 0, sizeof(pattern_generator_dirty));
  memset(name_table_dirty, 0, sizeof(name_table_dirty));
  pattern_generator_dirty[0] = true;
  any_pattern_generator_dirty = true;
  any_name_table_dirty = false;
  xpt = 0;
  ypt = 0;
}

FMT::FMT(Screen &screen_, const byte *data_, unsigned length_) :
  screen(screen_), data(data_), length(length_), offset(0)
{
  run(0);
}

unsigned FMT::run(unsigned offs)
{
  while (offs < length) {
    byte op = data[offs++];
    if (op == 0xfb) {
      /* fend/next */
      if (offs + 2 > length)
	/* short operand or fend */
	break;
      return offs;
    } else if (op < 0xfb) {
      byte cnt = (op & 0x1f)+1;
      op >>= 5;
      if (op < 2) {
	if (offs+cnt > length)
	  /* short operand */
	  break;
	byte xpt = screen.getXpt();
	byte ypt = screen.getYpt();
	if (op == 0) {
	  /* hstr */
	  screen.hstr(ypt, xpt, data+offs, cnt, offset);
	  if ((xpt += cnt) >= 32) {
	    xpt -= 32;
	    if (++ypt >= 24)
	      ypt -= 24;
	  }
	} else {
	  /* vstr */
	  screen.vstr(ypt, xpt, data+offs, cnt, offset);
	  if ((ypt += cnt) >= 24) {
	    ypt -= 24;
	    if (ypt >= 24)
	      ypt -= 24;
	  }
	}
	screen.setXpt(xpt);
	screen.setYpt(ypt);
	offs += cnt;
      } else if (op == 7)
	/* unsupported mode (hstr) */
	break;
      else if (op >= 4) switch(op) {
	case 4:
	  /* col+ */
	  {
	    byte xpt = screen.getXpt();
	    if ((xpt += cnt) >= 32) {
	      byte ypt = screen.getYpt();
	      xpt -= 32;
	      if (++ypt >= 24)
		ypt = 0;
	      screen.setYpt(ypt);
	    }
	    screen.setXpt(xpt);
	  }
	  break;
	case 5:
	  /* row+ */
	  {
	      byte ypt = screen.getYpt();
	      if ((ypt += cnt) >= 24) {
		ypt -= 24;
		if (ypt > 24)
		  ypt -= 24;
	      }
	      screen.setYpt(ypt);
	  }
	  break;
	case 6:
	  /* rptb */
	  do {
	    unsigned next = run(offs);
	    if (next >= length)
	      return next;
	    if (--cnt)
	      offs = (data[next]<<8)|data[next+1];
	    else
	      offs = next+2;
	  } while(cnt);
	  break;
      } else if (offs >= length)
	/* missing operand */
	break;
      else {
	byte ch = data[offs++] + offset;
	byte xpt = screen.getXpt();
	byte ypt = screen.getYpt();
	if (op == 2) {
	  /* hcha */
	  screen.hchar(ypt, xpt, ch, cnt);
	  if ((xpt += cnt) >= 32) {
	    xpt -= 32;
	    if (++ypt >= 24)
	      ypt -= 24;
	  }
	} else {
	  /* vcha */
	  screen.vchar(ypt, xpt, ch, cnt);
	  if ((ypt += cnt) >= 24) {
	    ypt -= 24;
	    if (ypt >= 24)
	      ypt -= 24;
	  }
	}
	screen.setXpt(xpt);
	screen.setYpt(ypt);
      }
    } else {
      if (op == 0xfd || offs >= length)
	/* unsupported mode (scro) or missing operand */
	break;
      byte arg = data[offs++];
      switch(op) {
      case 0xfc:
	/* scro */
	offset = arg;
	break;
      case 0xfe:
	/* row */
	screen.setYpt(arg);
	break;
      case 0xff:
	/* col */
	screen.setXpt(arg);
	break;
      }
    }
  }
  /* end of script or error */
  return length;
}

}}