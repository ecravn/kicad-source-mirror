/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2016 Cirilo Bernardo <cirilo.bernardo@gmail.com>
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
#include "vrml1_material.h"
#include "plugins/3dapi/ifsg_all.h"


WRL1MATERIAL::WRL1MATERIAL( NAMEREGISTER* aDictionary ) : WRL1NODE( aDictionary )
{
    colors[0] = NULL;
    colors[1] = NULL;
    m_Type = WRL1_MATERIAL;
    return;
}


WRL1MATERIAL::WRL1MATERIAL( NAMEREGISTER* aDictionary, WRL1NODE* aParent ) :
    WRL1NODE( aDictionary )
{
    colors[0] = NULL;
    colors[1] = NULL;
    m_Type = WRL1_MATERIAL;
    m_Parent = aParent;

    if( NULL != m_Parent )
        m_Parent->AddChildNode( this );

    return;
}


WRL1MATERIAL::~WRL1MATERIAL()
{
    #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 2 )
    std::cerr << " * [INFO] Destroying Material node\n";
    #endif

    // destroy any orphaned color nodes
    for( int i = 0; i < 2; ++i )
    {
        if( NULL != colors[i] )
        {
            #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 2 )
            std::cerr << " * [INFO] Destroying SGCOLOR #" << i << "\n";
            #endif

            if( NULL == S3D::GetSGNodeParent( colors[i] ) )
                S3D::DestroyNode( colors[i] );

            #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 2 )
            std::cerr << " * [INFO] destroyed SGCOLOR #" << i << "\n";
            #endif
        }
    }

    return;
}


bool WRL1MATERIAL::AddRefNode( WRL1NODE* aNode )
{
    // this node may not own or reference any other node

    #ifdef DEBUG_VRML1
    std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
    std::cerr << " * [BUG] AddRefNode is not applicable\n";
    #endif

    return false;
}


bool WRL1MATERIAL::AddChildNode( WRL1NODE* aNode )
{
    // this node may not own or reference any other node

    #ifdef DEBUG_VRML1
    std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
    std::cerr << " * [BUG] AddChildNode is not applicable\n";
    #endif

    return false;
}


bool WRL1MATERIAL::Read( WRLPROC& proc, WRL1BASE* aTopNode )
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

        if( !proc.ReadName( glob ) )
        {
            #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
            std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
            std::cerr << proc.GetError() <<  "\n";
            #endif

            return false;
        }

        // expecting one of:
        // ambientColor
        // diffuseColor
        // emissiveColor
        // shininess
        // specularColor
        // transparency

        proc.GetFilePosData( line, column );

        if( !glob.compare( "specularColor" ) )
        {
            if( !proc.ReadMFVec3f( specularColor ) )
            {
                #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
                std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
                std::cerr << " * [INFO] invalid specularColor at line " << line << ", column ";
                std::cerr << column << "\n";
                std::cerr << " * [INFO] file: '" << proc.GetFileName() << "'\n";
                std::cerr << " * [INFO] message: '" << proc.GetError() << "'\n";
                #endif

                return false;
            }
        }
        else if( !glob.compare( "diffuseColor" ) )
        {
            if( !proc.ReadMFVec3f( diffuseColor ) )
            {
                #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
                std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
                std::cerr << " * [INFO] invalid diffuseColor at line " << line << ", column ";
                std::cerr << column << "\n";
                std::cerr << " * [INFO] file: '" << proc.GetFileName() << "'\n";
                std::cerr << " * [INFO] message: '" << proc.GetError() << "'\n";
                #endif

                return false;
            }
        }
        else if( !glob.compare( "emissiveColor" ) )
        {
            if( !proc.ReadMFVec3f( emissiveColor ) )
            {
                #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
                std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
                std::cerr << " * [INFO] invalid emissiveColor at line " << line << ", column ";
                std::cerr << column << "\n";
                std::cerr << " * [INFO] file: '" << proc.GetFileName() << "'\n";
                std::cerr << " * [INFO] message: '" << proc.GetError() << "'\n";
                #endif

                return false;
            }
        }
        else if( !glob.compare( "shininess" ) )
        {
            if( !proc.ReadMFFloat( shininess ) )
            {
                #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
                std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
                std::cerr << " * [INFO] invalid shininess at line " << line << ", column ";
                std::cerr << column << "\n";
                std::cerr << " * [INFO] file: '" << proc.GetFileName() << "'\n";
                std::cerr << " * [INFO] message: '" << proc.GetError() << "'\n";
                #endif

                return false;
            }
        }
        else if( !glob.compare( "transparency" ) )
        {
            if( !proc.ReadMFFloat( transparency ) )
            {
                #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
                std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
                std::cerr << " * [INFO] invalid transparency at line " << line << ", column ";
                std::cerr << column << "\n";
                std::cerr << " * [INFO] file: '" << proc.GetFileName() << "'\n";
                std::cerr << " * [INFO] message: '" << proc.GetError() << "'\n";
                #endif

                return false;
            }
        }
        else if( !glob.compare( "ambientColor" ) )
        {
            if( !proc.ReadMFVec3f( ambientColor ) )
            {
                #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
                std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
                std::cerr << " * [INFO] invalid ambientColor at line " << line << ", column ";
                std::cerr << column << "\n";
                std::cerr << " * [INFO] file: '" << proc.GetFileName() << "'\n";
                std::cerr << " * [INFO] message: '" << proc.GetError() << "'\n";
                #endif

                return false;
            }
        }
        else
        {
            #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
            std::cerr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
            std::cerr << " * [INFO] bad Material at line " << line << ", column ";
            std::cerr << column << "\n";
            std::cerr << " * [INFO] file: '" << proc.GetFileName() << "'\n";
            #endif

            return false;
        }
    }   // while( true ) -- reading contents of Material{}

    return true;
}


SGNODE* WRL1MATERIAL::TranslateToSG( SGNODE* aParent, WRL1STATUS* sp )
{
    if( NULL == sp )
    {
        #if defined( DEBUG_VRML1 ) && ( DEBUG_VRML1 > 1 )
        std::cerr << " * [INFO] bad model: no base data given\n";
        #endif

        return NULL;
    }

    sp->mat = this;

    return NULL;
}


SGNODE* WRL1MATERIAL::GetAppearance( int aIndex )
{
    ++aIndex;

    // invalid indices result in the default colors
    if( aIndex != 0 && aIndex != 1 )
        aIndex = 0;

    if( NULL != colors[ aIndex ] )
        return colors[ aIndex ];

    IFSG_APPEARANCE app( true );

    float red, green, blue, val;

    if( aIndex == 0 || transparency.empty() )
        val = 0.0;
    else
        val = transparency[0];

    checkRange( val );
    app.SetTransparency( val );

    if( aIndex == 0 || shininess.empty() )
        val = 0.2;
    else
        val = shininess[0];

    checkRange( val );
    app.SetShininess( val );

    if( aIndex ==0 || ambientColor.empty() )
    {
        red = 0.2;
        green = 0.2;
        blue = 0.2;
    }
    else
    {
        red = ambientColor[0].x;
        green = ambientColor[0].y;
        blue = ambientColor[0].z;
    }

    checkRange( red );
    checkRange( green );
    checkRange( blue );
    val = (red + green + blue)/3.0;
    app.SetAmbient( val );

    if( aIndex == 0 || diffuseColor.empty() )
    {
        red = 0.8;
        green = 0.8;
        blue = 0.8;
    }
    else
    {
        red = diffuseColor[0].x;
        green = diffuseColor[0].y;
        blue = diffuseColor[0].z;
    }

    checkRange( red );
    checkRange( green );
    checkRange( blue );
    app.SetDiffuse( red, green, blue );

    if( aIndex > (int)emissiveColor.size() )
    {
        red = 0.0;
        green = 0.0;
        blue = 0.0;
    }
    else
    {
        red = emissiveColor[0].x;
        green = emissiveColor[0].y;
        blue = emissiveColor[0].z;
    }

    checkRange( red );
    checkRange( green );
    checkRange( blue );
    app.SetEmissive( red, green, blue );

    if( aIndex > (int)specularColor.size() )
    {
        red = 0.0;
        green = 0.0;
        blue = 0.0;
    }
    else
    {
        red = specularColor[0].x;
        green = specularColor[0].y;
        blue = specularColor[0].z;
    }

    checkRange( red );
    checkRange( green );
    checkRange( blue );
    app.SetSpecular( red, green, blue );

    colors[aIndex] = app.GetRawPtr();

    return colors[aIndex];
}


void WRL1MATERIAL::GetColor( SGCOLOR* aColor, int aIndex )
{
    if( NULL == aColor )
        return;

    // Calculate the color based on the given index.
    // If the index points to a valid diffuse and emissive colors,
    // take the higher value of each component.

    float red, blue, green;
    float eRed, eBlue, eGreen;

    if( aIndex < 0 || ( aIndex >= (int)diffuseColor.size()
        && aIndex >= (int)emissiveColor.size() ) )
    {
        // If the index is out of bounds, use the default diffuse color.
        red = 0.8;
        green = 0.8;
        blue = 0.8;
        aColor->SetColor( red, green, blue );
        return;
    }

    if( aIndex >= (int)diffuseColor.size() )
    {
        red = emissiveColor[aIndex].x;
        green = emissiveColor[aIndex].y;
        blue = emissiveColor[aIndex].z;

        checkRange( red );
        checkRange( green );
        checkRange( blue );

        aColor->SetColor( red, green, blue );
        return;
    }

    if( aIndex >= (int)emissiveColor.size() )
    {
        red = diffuseColor[aIndex].x;
        green = diffuseColor[aIndex].y;
        blue = diffuseColor[aIndex].z;

        checkRange( red );
        checkRange( green );
        checkRange( blue );

        aColor->SetColor( red, green, blue );
        return;
    }

    red = diffuseColor[aIndex].x;
    green = diffuseColor[aIndex].y;
    blue = diffuseColor[aIndex].z;

    eRed = emissiveColor[aIndex].x;
    eGreen = emissiveColor[aIndex].y;
    eBlue = emissiveColor[aIndex].z;

    checkRange( red );
    checkRange( green );
    checkRange( blue );

    checkRange( eRed );
    checkRange( eGreen );
    checkRange( eBlue );

    if( eRed > red )
        red = eRed;

    if( eGreen > green )
        green = eGreen;

    if( eBlue > blue )
        blue = eBlue;

    aColor->SetColor( red, green, blue );

    return;
}


void WRL1MATERIAL::checkRange( float& aValue )
{
    if( aValue < 0.0 )
        aValue = 0.0;
    else if( aValue > 1.0 )
        aValue = 1.0;

    return;
}


void WRL1MATERIAL::Reclaim( SGNODE* aColor )
{
    if( NULL == aColor )
        return;

    if( aColor == colors[0] )
    {
        if( NULL == S3D::GetSGNodeParent( aColor ) )
        {
            colors[0] = NULL;
            S3D::DestroyNode( aColor );
        }

        return;
    }

    if( aColor == colors[1] && NULL == S3D::GetSGNodeParent( aColor ) )
    {
        colors[1] = NULL;
        S3D::DestroyNode( aColor );
    }

    return;
}
