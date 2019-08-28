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
 * crud.hpp
 */

#ifndef _CRUD_HPP_
#define _CRUD_HPP_

#ident "$Id$"

#include <list>
#include <map>
#include <string>
#include <vector>

namespace cubcrud
{
  template <typename T>
  class property
  {
    public:
      std::string m_name;
      T& m_value_storage;
    public:
      property (const std::string &name, T &val);
      property (const char *name, T &val);

      const std::string &get_name () const { return m_name; }
      std::string get_value () const { return std::string (m_value_storage); }
  };

  template <typename T>
  property<T>::property (const std::string &name, T &val)
  : m_name (name)
  , m_value_storage (val)
    {
    }

  template <typename T>
  property<T>::property (const char *name, T &val)
   : m_value_storage (val)
    {
    if (name != NULL)
      {
        m_name = name;
      }
    }

  class table;

  class node
  {
    protected:
      std::string m_full_name;

      std::list<node *> m_children;
      node *m_parent;

    public:
      node ();

      void add_child (node *obj);

      virtual int get_table_columns (table &tbl) = 0;
      virtual int fill_table (table &tbl) = 0;
  };

  template <typename T>
  class object : public node
  {
    protected:
      property<T> m_property;

    public:
      object () = delete;
      object (const std::string &name, T &val);
      object (const char *name, T &val);

      int get_table_columns (table &tbl) override;
      int fill_table (table &tbl) override;
  };

  node::node ()
  : m_parent (NULL)
    {
    }

  void node::add_child (node *obj)
  {
    m_children.emplace_back (obj);
    obj->m_parent = this;
  }

  template <typename T>
  object<T>::object (const std::string &name, T &val)
  : m_property (name, val)
    {
    }

  template <typename T>
  object<T>::object (const char *name, T &val)
  : m_property (name, val)
    {
    }


  // generation of table data from a crud tree structure
  class row
    {
      private:
        std::vector<std::string> m_values;
    };

  class table
    {
      private:
        std::map<std::string, int> m_header;
        std::vector<row> m_rows;

        row m_current_row;

        int m_col_cnt;

     public:
        void add_column_name (const std::string &col_name);

        void end_row ();
        void add_column_value (const std::string &name, std::string &value);
    };
    
} /* namespace cubcrud */

#endif /* _CRUD_HPP_ */
