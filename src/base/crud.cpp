/*
 * Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

/*
 * crud.cpp
 */

#ident "$Id$"

#include "crud.hpp"

#include <assert.h>

namespace cubcrud
{
  template <typename T>
  int object<T>::get_table_columns (table &tbl)
  {
    tbl.add_column_name (m_property.get_name ());
    for auto c : m_children
      {
        c->get_table_columns (tbl);
      }
  }

  template <typename T>
  int object<T>::fill_table (table &tbl)
  {
    tbl.add_column_value (m_property.get_name (), m_property.get_value ());
    for auto c : m_children
      {
        c->fill_table (tbl);
      }
  }

  void table::add_column_name (const std::string &col_name)
  {
    assert (m_rows.size () == 0);

    int col_id = m_header.size ();
    m_header.insert (std::make_pair (col_name, col_id));
  }

  void table::add_column_value (const std::string &name, std::string &value)
  {
    assert (m_current_row.find (name) == m_current_row.end ());
    m_current_row[name] = value;
  }

} /* namespace cubcrud */
