/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CGstElement.hpp
 * @date    16.07.16
 * @author  Hlieb Romanov
 * @brief   Base class for all gstreamer element wrappers
 ************************************************************************/
#pragma once

#include <string>
#include "CGstPad.hpp"

/**
 * Base class for all GStreamer element wrapper classes.
 */
class CGstElement
{
public:
   CGstElement( void );
   virtual ~CGstElement( void );

   /**
    * Construct element from the factory name for example "audiotestsrc"
    */
   explicit CGstElement( const std::string& factoryName );

   /**
    * Wrap raw GstElement pointer.
    * @param element - raw pointer
    * @param takeOwnership - if set to true does not make ref() on raw pointer
    */
   explicit CGstElement( GstElement* element, bool takeOwnership = false );

   CGstElement( const CGstElement& other );
   virtual CGstElement& operator=( CGstElement other );
   friend void swap( CGstElement& first, CGstElement& second );

   bool isValid( void ) const
   {
      return ( mElement != NULL );
   }

   /**
    * Get name of this element
    */
   std::string getName( void ) const;

   /**
    * Link this element to another one.
    */
   virtual bool link( CGstElement& other );

   /**
    * Unlink this element from another one
    */
   virtual void unlink( CGstElement& other );

   /**
    * Get source pad of this element ( if exists ).
    * Don't forget to check pad.isValid(), because some elements don't have src pads.
    * @return source pad of the element.
    */
   virtual CGstPad& getSrcPad( void );

   /**
    * Get sink pad of this element ( if exists ).
    * Don't forget to check pad.isValid(), because some elements don't have sink pads.
    * @return sink pad of the element
    */
   virtual CGstPad& getSinkPad( void );

   /**
    * Send event to this element
    * @param event - raw GStreamer event pointer
    * @return true if event was handled
    */
   bool sendEvent( GstEvent* event );

   /**
    * Set the state of this element.
    * @param state - the value from GstState enum
    * @param async - if set to true this method will return immediately 
    * without waiting for the actual state change.
    */
   bool setState( GstState state, bool async = false );

   /**
    * Get the state of this element.
    * Warning! It may take some time if this method is 
    * called when the element is doing the state change async.
    * @return state of this element
    */
   GstState getState( void ) const;

   /**
    * Check whether the element is in specified state or not.
    */
   bool isInState( GstState state ) const;

   /**
    * Get raw GStreamer element pointer
    */
   GstElement* raw( void )
   {
      return mElement;
   }

   /**
    * Get the element property value.
    * @param propertyName - the name of the property
    * @return property value
    */
   template <typename T>
   T getProperty( const std::string& propertyName ) const
   {
      T propValue = NULL;
      g_object_get( G_OBJECT( mElement ), propertyName.c_str(), &propValue, NULL );
      return propValue;
   }

   /**
    * Template specialization for string properties
    * @sa getProperty()
    */
   template <>
   std::string CGstElement::getProperty<std::string>( const std::string& propertyName ) const;

   /**
    * Set the property value
    * @param propertyName - the property name
    * @param propertyValue - the property value
    */
   template <typename T>
   void setProperty( const std::string& propertyName, T propertyValue )
   {
      g_object_set( G_OBJECT( mElement ), propertyName.c_str(), propertyValue, NULL );
   }

   /**
    * Template specialization for string properties
    * @sa setProperty()
    */
   template <>
   void CGstElement::setProperty<const std::string&>( const std::string& propertyName, const std::string& propertyValue );


private:
   /**
    * Helper method to get the element name.
    */
   void extractName( void );

private:
   GstElement* mElement;
   CGstPad mSrcPad;
   CGstPad mSinkPad;
   std::string mName;
};
