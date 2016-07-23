/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CGstPad.cpp
 * @date    16.07.16
 * @author  Hlieb Romanov
 * @brief   GStreamer element pad wrapper
 ************************************************************************/
#include <cassert>
#include "CGstPad.hpp"

CGstPad::CGstPad( void )
   : mPad( NULL )
{
}

CGstPad::~CGstPad( void )
{
   if ( isValid() )
   {
      gst_object_unref( mPad );
   }
}

CGstPad::CGstPad( GstPad* pad, bool takeOwnership )
   : mPad( pad )
{
   if ( isValid() )
   {
      if ( !takeOwnership )
      {
         gst_object_ref( mPad );
      }
      extractName();
   }
}


CGstPad::CGstPad( const CGstPad& other )
   : mPad( NULL )
{
   if ( other.isValid() )
   {
      mPad = GST_PAD( gst_object_ref( other.mPad ) );
      mName.assign( other.mName );
      mPadProbeFunc = other.mPadProbeFunc;
   }
}

CGstPad& CGstPad::operator=( CGstPad other )
{
   CGstPad temp( other );
   swap( *this, temp );
   return *this;
}

void swap( CGstPad& first, CGstPad& second )
{
   using std::swap;
   std::swap( first.mPad, second.mPad );
   std::swap( first.mName, second.mName );
   std::swap( first.mPadProbeFunc, second.mPadProbeFunc );
}

bool CGstPad::isValid( void ) const
{
   return ( mPad != NULL );
}

std::string CGstPad::getName( void ) const
{
   assert( isValid() );
   return mName;
}

CGstPad CGstPad::getPeerPad( void ) const
{
   assert( isValid() );
   return CGstPad( gst_pad_get_peer( mPad ), true );
}

bool CGstPad::sendEvent( GstEvent* event )
{
   assert( isValid() );
   return ( gst_pad_send_event( mPad, event ) != FALSE );
}

bool CGstPad::link( CGstPad& other )
{
   assert( isValid() );
   assert( other.isValid() );

   return ( gst_pad_link( mPad, other.mPad ) == GST_PAD_LINK_OK );
}

bool CGstPad::unlink( CGstPad& other )
{
   assert( isValid() );
   assert( other.isValid() );
   return ( gst_pad_unlink( mPad, other.mPad ) != FALSE );
}

GstPad* CGstPad::raw( void )
{
   assert( isValid() );
   return mPad;
}

unsigned long CGstPad::addProbe( GstPadProbeType mask, const GstPadProbeFunc& callback )
{
   mPadProbeFunc = callback;
   return gst_pad_add_probe( mPad, mask, probeCallback, this, NULL );
}

void CGstPad::removeProbe( unsigned long probeId )
{
   gst_pad_remove_probe( mPad, probeId );
}

void CGstPad::extractName( void )
{
   assert( isValid() );
   char* name = gst_pad_get_name( mPad );
   mName.assign( name );
   g_free( name ); 
}

GstPadProbeReturn CGstPad::probeCallback( GstPad* pad, GstPadProbeInfo* info, gpointer user_data )
{
   assert( user_data != NULL );
   GstPadProbeReturn result = GST_PAD_PROBE_PASS;
   CGstPad* self = reinterpret_cast<CGstPad*>( user_data );
   if ( self->mPadProbeFunc )
   {
      result = self->mPadProbeFunc( info );
   }
   return result;
}