////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/RenderImage.hpp>
#include <SFML/Graphics/RenderImageImplFBO.hpp>
#include <SFML/Graphics/RenderImageImplPBuffer.hpp>
#include <iostream>


namespace sf
{
////////////////////////////////////////////////////////////
/// Default constructor
////////////////////////////////////////////////////////////
RenderImage::RenderImage() :
myRenderImage(NULL)
{

}


////////////////////////////////////////////////////////////
/// Destructor
////////////////////////////////////////////////////////////
RenderImage::~RenderImage()
{
    SetActive(false);
    delete myRenderImage;
}


////////////////////////////////////////////////////////////
/// Create the render image from its dimensions
////////////////////////////////////////////////////////////
bool RenderImage::Create(unsigned int width, unsigned int height, bool depthBuffer)
{
    // Make sure that render-images are supported
    if (!CanUseRenderImage())
    {
        std::cerr << "Impossible to create render image (your system doesn't support this feature)" << std::endl;
        return false;
    }

    // Create the image
    if (!myImage.Create(width, height))
    {
        std::cerr << "Impossible to create render image (failed to create the target image)" << std::endl;
        return false;
    }

    // Disable smoothing -- we don't want to alter the original rendering
    myImage.SetSmooth(false);

    // Create the implementation
    delete myRenderImage;
    if (priv::RenderImageImplFBO::IsSupported())
    {
        // Use FBO
        myRenderImage = new priv::RenderImageImplFBO;
    }
    else if (priv::RenderImageImplPBuffer::IsSupported())
    {
        // Use P-Buffer
        myRenderImage = new priv::RenderImageImplPBuffer;
    }

    // Initialize the render image
    if (!myRenderImage->Create(width, height, myImage.myTexture, depthBuffer))
        return false;

    // We can now initialize the render target part
    RenderTarget::Initialize();

    return true;
}


////////////////////////////////////////////////////////////
/// Activate of deactivate the render-image as the current target
/// for rendering
////////////////////////////////////////////////////////////
bool RenderImage::SetActive(bool active)
{
    return myRenderImage && myRenderImage->Activate(active);
}


////////////////////////////////////////////////////////////
/// Update the contents of the target
////////////////////////////////////////////////////////////
void RenderImage::Display()
{
    // Render everything that has been drawn so far
    Flush();

    // Update the target image
    if (myRenderImage)
    {
        bool pixelsFlipped = myRenderImage->UpdateTexture(myImage.myTexture);
        myImage.myPixelsFlipped = pixelsFlipped;
        myImage.myNeedArrayUpdate = true;
        myImage.myNeedTextureUpdate = false;
    }
}


////////////////////////////////////////////////////////////
/// Get the width of the rendering region of the image
////////////////////////////////////////////////////////////
unsigned int RenderImage::GetWidth() const
{
    return myImage.GetWidth();
}


////////////////////////////////////////////////////////////
/// Get the height of the rendering region of the image
////////////////////////////////////////////////////////////
unsigned int RenderImage::GetHeight() const
{
    return myImage.GetHeight();
}


////////////////////////////////////////////////////////////
/// Get the target image
////////////////////////////////////////////////////////////
const Image& RenderImage::GetImage() const
{
    return myImage;
}


////////////////////////////////////////////////////////////
/// Check whether the system supports render images or not
////////////////////////////////////////////////////////////
bool RenderImage::CanUseRenderImage()
{
    return priv::RenderImageImplFBO::IsSupported() ||
           priv::RenderImageImplPBuffer::IsSupported();
}


////////////////////////////////////////////////////////////
/// Activate / deactivate the render image for rendering
////////////////////////////////////////////////////////////
bool RenderImage::Activate(bool active)
{
    return SetActive(active);
}

} // namespace sf