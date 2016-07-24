/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CGstElement.cpp
 * @date    17.07.16
 * @author  Hlieb Romanov
 * @brief   Base class for all gstreamer element wrappers
 ************************************************************************/
#include "CGstElement.hpp"

static const guint64 MAX_WAIT_TIME = 5 * GST_SECOND;

CGstElement::CGstElement( void )
   : mElement( NULL )
   , mSrcPad()
   , mSinkPad()
   , mName()
{
}

CGstElement::~CGstElement( void )
{
   if ( isValid() )
   {
      gst_object_unref( mElement );
   }
}

CGstElement::CGstElement( const std::string& factoryName )
   : mElement( gst_element_factory_make( factoryName.c_str(), NULL ) )
   , mSrcPad()
   , mSinkPad()
{
   if ( isValid() )
   {
      mSrcPad = CGstPad( gst_element_get_static_pad( mElement, "src" ), true );
      mSinkPad = CGstPad( gst_element_get_static_pad( mElement, "sink" ), true );
      extractName();
   }
}

CGstElement::CGstElement( GstElement* element, bool takeOwnership )
   : mElement( element )
   , mSrcPad()
   , mSinkPad()
{
   if ( isValid() )
   {
      if ( !takeOwnership )
      {
         gst_object_ref( element );
      }
      mSrcPad = CGstPad( gst_element_get_static_pad( mElement, "src" ), true );
      mSinkPad = CGstPad( gst_element_get_static_pad( mElement, "sink" ), true );
      extractName();
   }
}


CGstElement::CGstElement( const CGstElement& other )
   : mElement( NULL )
{
   if ( isValid() )
   {
      mElement = GST_ELEMENT( gst_object_ref( other.mElement ) );
      mSrcPad = other.mSrcPad;
      mSinkPad = other.mSinkPad;
      mName = other.mName;
   }
}

CGstElement& CGstElement::operator=( CGstElement other )
{
   swap( *this, other );
   return *this;
}

void swap( CGstElement& first, CGstElement& second )
{
   using std::swap;
   std::swap( first.mElement, second.mElement );
   std::swap( first.mSrcPad, second.mSrcPad );
   std::swap( first.mSinkPad, second.mSinkPad );
   std::swap( first.mName, second.mName );
}


std::string CGstElement::getName( void ) const
{
   return mName;
}

bool CGstElement::link( CGstElement& other )
{
   assert( isValid() );
   assert( other.isValid() );
   return getSrcPad().link( other.getSinkPad() );
}

void CGstElement::unlink( CGstElement& other )
{
   assert( isValid() );
   assert( other.isValid() );
   getSrcPad().unlink( other.getSinkPad() );
}


CGstPad& CGstElement::getSrcPad( void )
{
   return mSrcPad;
}

CGstPad& CGstElement::getSinkPad( void )
{
   return mSinkPad;
}


bool CGstElement::sendEvent( GstEvent* event )
{
   assert( isValid() );
   return ( gst_element_send_event( mElement, event ) != FALSE );
}


bool CGstElement::setState( GstState state, bool async )
{
   assert( isValid() );
   bool result = false;
   GstStateChangeReturn retval = gst_element_set_state( mElement, state );
   if ( async )
   {
      result = ( retval != GST_STATE_CHANGE_FAILURE );
   }
   else
   {
      if ( retval != GST_STATE_CHANGE_FAILURE )
      {
         result = isInState( state );
      }
   }
   return result;
}

GstState CGstElement::getState( void ) const
{
   assert( isValid() );
   GstState result = GST_STATE_VOID_PENDING;
   GstState pending = GST_STATE_VOID_PENDING;
   gst_element_get_state( mElement, &result, &pending, MAX_WAIT_TIME );
   return result;
}

bool CGstElement::isInState( GstState state ) const
{
   assert( isValid() );
   return ( state == getState() );
}

void CGstElement::extractName( void )
{
   assert( isValid() );
   char* name = gst_element_get_name( mElement );
   mName.assign( name );
   g_free( name );
}