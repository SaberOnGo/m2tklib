/*

  m2elslbase.c

  base library for "strlist"

  m2tklib = Mini Interative Interface Toolkit Library
  
  Copyright (C) 2011  olikraus@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
  
  slbase specific options:
    l:			Number of lines (defaults to 1)
  
*/

#include "m2.h"

/*==============================================================*/
/* slbase member access procedures */

uint8_t *m2_el_slbase_get_len_ptr(m2_rom_void_p element)
{
  return (uint8_t *)m2_rom_get_ram_ptr(element, offsetof(m2_el_slbase_t, len));
}

uint8_t m2_el_slbase_get_len(m2_rom_void_p element)
{
  return *m2_el_slbase_get_len_ptr(element);
}


uint8_t *m2_el_slbase_get_top_ptr(m2_rom_void_p element)
{
  return (uint8_t *)m2_rom_get_ram_ptr(element, offsetof(m2_el_slbase_t, top));
}

uint8_t m2_el_slbase_get_top(m2_rom_void_p element)
{
  return *m2_el_slbase_get_top_ptr(element);
}

/*==============================================================*/
/* slbase utility procedures */

/*
  return value from 0 .. cX-1 if the 
  based on the number of visible lines, the top visible element and the total number of lines, calculate if the 
  current (fn->arg) is visible. Also calc the position

  visible_lines  0...
  total_lines 0...
  top_line	0...total_lines-1
  curr_line (fn_arg->arg) 0...total_lines-1

  if ( curr_line < top_line )
    not visible
  if ( curr_line >= top_line + visible_lines )
    not_visible
  vis_pos = curr_line - top_line  
*/


/* argument must be of type m2_el_slbase_p */ 
uint8_t m2_el_slbase_get_visible_lines(m2_rom_void_p element)
{
  return m2_el_fmfmt_opt_get_val_any_by_element(element, 'l', 1);
}

/*
  Calculate the visible position, based on the internal index
  position. Order of the result is inverted, 0 is botton element.
  Return M2_EL_SLBASE_ILLEGAL if the value is not visible.
*/
/* argument must be of type m2_el_slbase_p */ 
uint8_t m2_el_slbase_get_visible_pos(m2_rom_void_p element, uint8_t idx)
{
  uint8_t top = m2_el_slbase_get_top(element);
  uint8_t lines;
  if ( idx < top )
    return M2_EL_SLBASE_ILLEGAL;
  idx -= top;
  lines = m2_el_slbase_get_visible_lines(element);
  if ( idx >= lines )
    return M2_EL_SLBASE_ILLEGAL;
  lines--;
  lines -= idx;
  return lines;
}

/* argument must be of type m2_el_slbase_p */ 
uint8_t m2_el_slbase_calc_height(m2_rom_void_p element)
{
  uint8_t h;
  uint8_t visible_lines = m2_el_slbase_get_visible_lines(element);
  h = m2_gfx_get_char_height_with_normal_border(m2_el_fmfmt_get_font_by_element(element));
  h *= visible_lines;
  h = m2_calc_vlist_height_overlap_correction(h, visible_lines);
  return h;
}

/* argument must be of type m2_el_slbase_p */ 
uint8_t m2_el_slbase_calc_width(m2_rom_void_p element)
{
  return m2_gfx_add_normal_border_width(m2_el_fmfmt_get_font_by_element(element), m2_el_fnfmt_get_wW_by_element(element));
}

void m2_el_slbase_adjust_top_to_focus(m2_rom_void_p element, uint8_t pos)
{
  uint8_t top = m2_el_slbase_get_top(element);
  uint8_t lines;
  if ( pos < top )
  {
    top = pos;
  }
  else 
  {
    lines = m2_el_slbase_get_visible_lines(element);
    if ( pos >= top+lines )
    {
      top = pos;
      top -= lines;
      top++;
    }
  }
  *m2_el_slbase_get_top_ptr(element) = top;
}

void m2_el_slbase_adjust_top_to_cnt(m2_rom_void_p element)
{
  uint8_t cnt = m2_el_slbase_get_len(element);
  uint8_t *top = m2_el_slbase_get_top_ptr(element);
  if ( *top >= cnt )
  {
    if ( cnt > 0 )
      cnt--;
    *top = cnt;
  }
}
 
void m2_el_slbase_calc_box(m2_rom_void_p el_slbase, uint8_t idx, m2_pcbox_p data)
{
  uint8_t visible_pos = m2_el_slbase_get_visible_pos(el_slbase, idx);
  uint8_t y;
  data->c.y = data->p.y;
  data->c.x = data->p.x;
  
  if ( visible_pos == M2_EL_SLBASE_ILLEGAL )
    return;
  
  y = m2_gfx_get_char_height_with_normal_border(m2_el_fmfmt_get_font_by_element(el_slbase));
  y *= visible_pos;
  visible_pos++;  /* for the overlap correction */
  y = m2_calc_vlist_height_overlap_correction(y, visible_pos);
  data->c.y += y;
#ifdef M2_EL_MSG_DBG_SHOW
  printf("- slbase calc box: idx:%d pos:%d cx:%d cy:%d px:%d py:%d\n", idx, visible_pos, data->c.x, data->c.y, data->p.x, data->p.y);
#endif
}
