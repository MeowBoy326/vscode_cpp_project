//------------------------------------------------------------------------------
/*

   Copyright 2017, Nigel Atkinson

License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
//==============================================================================

/* Special members.
 * ----------------
 *  In addition to assigning a function to the likes of __tostring and other built
 *  in Lua metamethods, three additional members can be used.
 *
 *  __arrayindex    This is called to provide a result when an object is indexed
 *                  with a number. E.g. a = obj[5]
 *
 *  __arraynewindex This is called to store or otherwise use a value when assigned
 *                  to an object via a numerical index. E.g. obj[7] = a
 *
 *  __upcast        This will be called by LuaBindingUpCast to return a shared
 *                  pointer to a parent class, allowing for polymorphic use.
 *                  See upcast in the examples.
 *
 */


#ifndef BINDING_H
#define BINDING_H
#include <memory>
#include "lua.hpp"
#include "LuaN4502.h"
#include "LuaException.h"

template< bool cond, typename U >
using enable_if_t = typename std::enable_if< cond, U >::type;

namespace ManualBind {

// traits

// constructor
template <typename T>
using create_t = decltype(std::declval<T>().create(std::declval<lua_State*>()));
template <typename T>
using has_create = detect<T, create_t>;

// members
template <typename T>
using members_t = decltype(std::declval<T>().members());
template <typename T>
using has_members = detect<T, members_t>;

// properties
template <typename T>
using properties_t = decltype(std::declval<T>().properties());
template <typename T>
using has_properties = detect<T, properties_t>;

// any other stuff you might want to add to the metatable.
template <typename T>
using extras_t = decltype(std::declval<T>().setExtraMeta(std::declval<lua_State*>()));
template <typename T>
using has_extras = detect<T, extras_t>;

// helper struct
struct bind_properties {
    const char *name;
    lua_CFunction getter;
    lua_CFunction setter;
};

// Called when Lua object is indexed: obj[ndx]
int LuaBindingIndex( lua_State *L );
// Called whe Lua object index is assigned: obj[ndx] = blah
int LuaBindingNewIndex( lua_State *L );

// Gets the table of extra values assigned to an instance.
int LuaBindGetExtraValuesTable( lua_State* L, int index );

void LuaBindingSetProperties( lua_State *L, bind_properties* properties );

// If the object at 'index' is a userdata with a metatable containing a __upcast
// function, then replaces the userdata at 'index' in the stack with the result
// of calling __upcast.
// Otherwise the object at index is replaced with nil.
int LuaBindingUpCast( lua_State* L, int index );

// Check the number of arguments are as expected.
// Throw an error if not.
void CheckArgCount( lua_State *L, int expected );

// B - the binding class / struct
// T - the class you are binding to Lua.

// Shared pointer version
// Use this for classes that need to be shared between C++ and Lua,
// or are expensive to copy. Think of it as like "by Reference".
template<class B, class T>
struct Binding {

    // Push the object on to the Lua stack
    static void push( lua_State *L, const std::shared_ptr<T>& sp )
    {

        if( sp == nullptr ) {
            lua_pushnil( L );
            return;
        }

        void *ud = lua_newuserdata( L, sizeof(std::shared_ptr<T>));

        new(ud) std::shared_ptr<T>( sp );

        luaL_setmetatable( L, B::class_name );
    }

    static void setMembers( lua_State* L, std::true_type )
    {
        luaL_setfuncs( L, B::members(), 0 );
    }

    static void setMembers( lua_State* L, std::false_type )
    {
        // Nada.
    }

    static void setProperties( lua_State* L, std::true_type )
    {
        bind_properties* props = B::properties();
        LuaBindingSetProperties( L, props );
    }

    static void setProperties( lua_State* L, std::false_type )
    {
        // Nada.
    }

    static void setExtras( lua_State* L, std::true_type )
    {
        B::setExtraMeta( L );
    }

    static void setExtras( lua_State*, std::false_type )
    {
        // Nada.
    }

    static int construct( lua_State* L, std::true_type )
    {
        // Remove table from stack.
        lua_remove( L, 1 );
        // Call create.
        return B::create( L );
    }

    static int construct( lua_State* L, std::false_type )
    {
        return luaL_error( L, "Can not create an instance of %s.", B::class_name);
    }

    static int construct( lua_State* L )
    {
        has_create<B> createTrait;
        return construct( L, createTrait);
    }

    static int pairs( lua_State* L )
    {
        lua_getglobal( L, "pairs" );
        luaL_getmetatable( L, B::class_name );
        lua_pcall( L, 1, LUA_MULTRET, 0 );
        return 3;
    }

    // Create metatable and register Lua constructor
    static void register_class( lua_State *L )
    {
        has_members<B> membersTrait;
        has_properties<B> propTrait;
        has_extras<B> extrasTrait;

        lua_newtable( L );  // Class access
        lua_newtable( L );  // Class access metatable

        luaL_newmetatable( L, B::class_name );
        setMembers( L, membersTrait );
        lua_pushcfunction( L, LuaBindingIndex );
        lua_setfield( L, -2, "__index" );
        lua_pushcfunction( L, LuaBindingNewIndex );
        lua_setfield( L, -2, "__newindex" );
        lua_pushcfunction( L, destroy );
        lua_setfield( L, -2, "__gc" );
        lua_newtable( L ); // __properties
        setProperties( L, propTrait );
        lua_setfield( L, -2, "__properties" );
        setExtras( L, extrasTrait );

        lua_setfield( L, -2, "__index" ); // Set metatable as index table.

        lua_pushcfunction( L, construct );
        lua_setfield( L, -2, "__call" );

        lua_pushcfunction( L, pairs );
        lua_setfield( L, -2, "__pairs" );

        lua_setmetatable( L, -2 );
        lua_setglobal( L, B::class_name );
    }

    //
    // Called when Lua object is garbage collected.
    static int destroy( lua_State *L )
    {
        void* ud = luaL_checkudata( L, 1, B::class_name );

        auto sp = static_cast<std::shared_ptr<T>*>(ud);

        // Explicitly called, as this was 'placement new'd
        sp->~shared_ptr();

        return 0;
    }

    // Grab object shared pointer from the Lua stack
    static const std::shared_ptr<T>& fromStack( lua_State *L, int index )
    {
        void* ud = luaL_checkudata( L, index, B::class_name );

        auto sp = static_cast<std::shared_ptr<T>*>(ud);

        return *sp;
    }

    static const std::shared_ptr<T>& fromStackThrow( lua_State *L, int index )
    {
        void* ud = luaL_testudata( L, index, B::class_name );

        if( ud == nullptr )
            throw LuaException( "Unexpected item on Lua stack." );

        auto sp = static_cast<std::shared_ptr<T>*>(ud);

        return *sp;
    }

};

// Plain Old Data POD version.
// Use this for simpler classes/structures where coping is fairly cheap, and
// C++ and Lua do not need to operate on the same instance.
// Think of this as "by Value"
template<class B, class T>
struct PODBinding {

    // Push the object on to the Lua stack
    static void push( lua_State *L, const T& value )
    {
        void *ud = lua_newuserdata( L, sizeof(T));

        new(ud) T( value );

        luaL_setmetatable( L, B::class_name );
    }

    static void setMembers( lua_State* L, std::true_type )
    {
        luaL_setfuncs( L, B::members(), 0 );
    }

    static void setMembers( lua_State*, std::false_type )
    {
        // Nada.
    }

    static void setProperties( lua_State* L, std::true_type )
    {
        bind_properties* props = B::properties();
        LuaBindingSetProperties( L, props );
    }

    static void setProperties( lua_State*, std::false_type )
    {
        // Nada.
    }

    static void setExtras( lua_State* L, std::true_type )
    {
        B::setExtraMeta( L );
    }

    static void setExtras( lua_State*, std::false_type )
    {
        // Nada.
    }

    static int construct( lua_State* L, std::true_type )
    {
        // Remove table from stack.
        lua_remove( L, 1 );
        // Call create.
        return B::create( L );
    }

    static int construct( lua_State* L, std::false_type )
    {
        return luaL_error( L, "Can not create an instance of %s.", B::class_name);
    }

    static int construct( lua_State* L )
    {
        has_create<B> createTrait;
        return construct( L, createTrait);
    }

    static int pairs( lua_State* L )
    {
        lua_getglobal( L, "pairs" );
        luaL_getmetatable( L, B::class_name );
        lua_pcall( L, 1, LUA_MULTRET, 0 );
        return 3;
    }

    // Create metatable and register Lua constructor
    static void register_class( lua_State *L )
    {
        has_members<B> membersTrait;
        has_properties<B> propTrait;
        has_extras<B> extrasTrait;

        lua_newtable( L );  // Class access
        lua_newtable( L );  // Class access metatable

        luaL_newmetatable( L, B::class_name );
        setMembers( L, membersTrait );
        lua_pushcfunction( L, LuaBindingIndex );
        lua_setfield( L, -2, "__index" );
        lua_pushcfunction( L, LuaBindingNewIndex );
        lua_setfield( L, -2, "__newindex" );
        //lua_pushcfunction( L, destroy );  -- if you need to destruct a POD
        //lua_setfield( L, -2, "__gc" );    -- set __gc to destory in the members.
        lua_newtable( L ); // __properties
        setProperties( L, propTrait );
        lua_setfield( L, -2, "__properties" );
        setExtras( L, extrasTrait );

        lua_setfield( L, -2, "__index" );

        lua_pushcfunction( L, construct );
        lua_setfield( L, -2, "__call" );

        lua_pushcfunction( L, pairs );
        lua_setfield( L, -2, "__pairs" );

        lua_setmetatable( L, -2 );
        lua_setglobal( L, B::class_name );
    }

    // This is still here should a POD really need 
    // destructing. Shouldn't be a common case.
    static int destroy( lua_State *L )
    {
        void* ud = luaL_checkudata( L, 1, B::class_name );

        auto p = static_cast<T*>(ud);

        // Explicitly called, as this was 'placement new'd
        p->~T();

        return 0;
    }

    // Grab object pointer from the Lua stack
    static T& fromStack( lua_State *L, int index )
    {
        void* ud = luaL_checkudata( L, index, B::class_name );

        auto p = static_cast<T*>(ud);

        return *p;
    }

    static T& fromStackThrow( lua_State *L, int index )
    {
        void* ud = luaL_testudata( L, index, B::class_name );

        if( ud == nullptr )
            throw LuaException( "Unexpected item on Lua stack." );

        auto p = static_cast<T*>(ud);

        return *p;
    }
};


}; // namespace ManualBind

#endif // BINDING_H
