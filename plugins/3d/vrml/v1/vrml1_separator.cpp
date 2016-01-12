/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015-2016 Cirilo Bernardo <cirilo.bernardo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <iostream>

#include "vrml1_base.h"
#include "vrml1_separator.h"
#include "plugins/3dapi/ifsg_all.h"


WRL1SEPARATOR::WRL1SEPARATOR( NAMEREGISTER* aDictionary ) : WRL1NODE( aDictionary )
{
    m_Type = WRL1_SEPARATOR;
    return;
}


WRL1SEPARATOR::WRL1SEPARATOR( NAMEREGISTER* aDictionary, WRL1NODE* aParent ) :
    WRL1NODE( aDictionary )
{
    m_Type = WRL1_SEPARATOR;
    m_Parent = aParent;

    if( NULL != m_Parent )
        m_Parent->AddChildNode( this );

    return;
}


WRL1SEPARATOR::~WRL1SEPARATOR()
{
    #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 2 )
    std::cerr << " * [INFO] Destroying Separator with " << m_Children.size();
    std::cerr << " children, " << m_Refs.size() << " references and ";
    std::cerr << m_BackPointers.size() << " backpointers\n";
    #endif

    return;
}


// functions inherited from WRL1NODE
bool WRL1SEPARATOR::Read( WRLPROC& proc, WRL1BASE* aTopNode )
{
    if( NULL == aTopNode )
    {
        #ifdef DEBUG_VRML1
        std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        std::cerr << " * [BUG] aTopNode is NULL\n";
        #endif

        return false;
    }

    size_t line, column;
    proc.GetFilePosData( line, column );

    char tok = proc.Peek();

    if( proc.eof() )
    {
        #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
        std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        std::cerr << " * [INFO] bad file format; unexpected eof at line ";
        std::cerr << line << ", column " << column << "\n";
        #endif

        return false;
    }

    if( '{' != tok )
    {
        #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
        std::cerr << proc.GetError() << "\n";
        std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        std::cerr << " * [INFO] bad file format; expecting '{' but got '" << tok;
        std::cerr  << "' at line " << line << ", column " << column << "\n";
        #endif

        return false;
    }

    proc.Pop();
    std::string glob;

    while( true )
    {
        if( proc.Peek() == '}' )
        {
            proc.Pop();
            break;
        }

        proc.GetFilePosData( line, column );

        if( !aTopNode->ReadNode( proc, this, NULL ) )
        {
            #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
            std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
            std::cerr << " * [INFO] bad file format; unexpected eof at line ";
            std::cerr << line << ", column " << column << "\n";
            #endif

            return false;
        }

        if( proc.Peek() == ',' )
            proc.Pop();

    }   // while( true ) -- reading contents of Separator{}

    return true;
}


SGNODE* WRL1SEPARATOR::TranslateToSG( SGNODE* aParent, WRL1STATUS* sp )
{
    #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 2 )
    std::cerr << " * [INFO] Translating Separator with " << m_Children.size();
    std::cerr << " children, " << m_Refs.size() << " references and ";
    std::cerr << m_BackPointers.size() << " backpointers (total ";
    std::cerr << m_Items.size() << " items)\n";
    #endif

    if( !m_Parent )
    {
        #ifdef DEBUG
        std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        std::cerr << " * [BUG] Separator has no parent\n";
        #endif

        return NULL;
    }

    if( sp != NULL )
        m_current = *sp;
    else
        m_current.Init();

    S3D::SGTYPES ptype = S3D::GetSGNodeType( aParent );

    if( NULL != aParent && ptype != S3D::SGTYPE_TRANSFORM )
    {
        #ifdef DEBUG_VRML1
        std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        std::cerr << " * [BUG] Separator does not have a Transform parent (parent ID: ";
        std::cerr << ptype << ")\n";
        #endif

        return NULL;
    }

    IFSG_TRANSFORM txNode( aParent );
    bool hasContent = false;

    std::list< WRL1NODE* >::iterator sI = m_Items.begin();
    std::list< WRL1NODE* >::iterator eI = m_Items.end();

    SGNODE* node = txNode.GetRawPtr();

    while( sI != eI )
    {
        if( NULL != (*sI)->TranslateToSG( node, &m_current ) )
            hasContent = true;

        ++sI;
    }

    if( !hasContent )
    {
        txNode.Destroy();
        return NULL;
    }

    return node;
}
