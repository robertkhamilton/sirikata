/*  Sirikata liboh -- Ogre Graphics Plugin
 *  CubeMap.cpp
 *
 *  Copyright (c) 2009, Daniel Reiter Horn
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <oh/Platform.hpp>
#include "OgreSystem.hpp"
#include "CameraEntity.hpp"
#include <Ogre.h>
#include "CubeMap.hpp"
namespace Sirikata { namespace Graphics {
CubeMap::CubeMap(OgreSystem*parent,const std::vector<String>&cubeMapTexture, int size, const std::vector<Vector3f>&cameraDelta){
    mFaceCounter=mMapCounter=0;
    mParent=parent;
    char cameraIdentifier;
    int num_mipmaps=0;
    for (int tsize=size;tsize;tsize/=2) {
        num_mipmaps++;
    }
    for (size_t i=0;i<cubeMapTexture.size();++i) {
        Ogre::ResourcePtr tmp=Ogre::TextureManager::getSingleton().getByName(cubeMapTexture[i]);
        if ( tmp.get()) {
            mCubeMapTextures.push_back(tmp);
        }else {            
            mCubeMapTextures.push_back(Ogre::TextureManager::getSingleton()
                                  .createManual(cubeMapTexture[i],
                                                Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                Ogre::TEX_TYPE_CUBE_MAP,
                                                size,
                                                size,
                                                num_mipmaps,
                                                0,
                                                Ogre::PF_A8R8G8B8,
                                                Ogre::TU_RENDERTARGET|Ogre::TU_AUTOMIPMAP));
        }
    }
    mBackbuffer = Ogre::TextureManager::getSingleton()
        .createManual(UUID::random().toString(),
                      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                      Ogre::TEX_TYPE_CUBE_MAP,
                      size,
                      size,
                      num_mipmaps,
                      0,
                      Ogre::PF_A8R8G8B8,
					  Ogre::TU_RENDERTARGET|Ogre::TU_AUTOMIPMAP);

    mCameraDelta=cameraDelta;
    mCamera=mParent->getSceneManager()->createCamera(mBackbuffer->getName()+"Camera");
    mCamera->setNearClipDistance(0.1f);
    mCamera->setAspectRatio(1);
    mCamera->setFOVy(Ogre::Radian(Ogre::Math::PI/2));

    for (unsigned int i=0;i<6;++i)
    {
        mFaces[i].mParent=this;
        
        Ogre::RenderTarget *renderTarget =mBackbuffer->getBuffer(i)->getRenderTarget(); //mBackbuffer->getBuffer(i)->getRenderTarget();
        renderTarget->addListener(&mFaces[i]);
    } 

}
bool CubeMap::frameEnded(const Ogre::FrameEvent&evt) {
    if (mFaceCounter>0&&mFaceCounter<7) {
        mBackbuffer->getBuffer(mFaceCounter-1)->getRenderTarget()->removeAllViewports();        
    }
    if (mFaceCounter<6) {
        Ogre::Viewport *viewport = mBackbuffer->getBuffer(mFaceCounter)->getRenderTarget()->addViewport( mCamera );        
        viewport->setOverlaysEnabled(false);
        viewport->setClearEveryFrame( true );
        viewport->setBackgroundColour( Ogre::ColourValue(1,0,0,1) );
    }
    if (mFaceCounter==7) {
        mBackbuffer->copyToTexture(mCubeMapTextures[mMapCounter]);        
    }
    ++mFaceCounter;
    if (mFaceCounter==8) {
        mFaceCounter=0;
        mMapCounter++;
        if ((size_t)mMapCounter==mCubeMapTextures.size()) {
            mMapCounter=0;
        }
    }
    
    return true;
}
void CubeMap::CubeMapFace::preRenderTargetUpdate(const Ogre::RenderTargetEvent&evt) {
    mParent->preRenderTargetUpdate(this-&mParent->mFaces[0],evt);
}
void CubeMap::preRenderTargetUpdate(int renderTargetIndex,const Ogre::RenderTargetEvent&evt) {
    mCamera->setPosition(toOgre(mParent->getPrimaryCamera()->getOgrePosition()+Vector3d(mCameraDelta[mMapCounter]),mParent->getOffset()));
    mCamera->setOrientation(Ogre::Quaternion::IDENTITY);
    switch (renderTargetIndex) {
      case 0:
        
        mCamera->yaw(-Ogre::Radian(Ogre::Math::PI/2));
        break;
      case 1:
        mCamera->yaw(Ogre::Radian(Ogre::Math::PI/2));
        break;
      case 2:
        mCamera->pitch(Ogre::Radian(Ogre::Math::PI/2));
        break;
      case 3:
        mCamera->pitch(-Ogre::Radian(Ogre::Math::PI/2));
        break;
      case 5:
         mCamera->yaw( Ogre::Radian( Ogre::Math::PI ) );
         break;
      case 4:
        break;
    }
}
CubeMap::~CubeMap() {
    for (unsigned int i=0;i<6;++i)
    {
        Ogre::RenderTarget *renderTarget = mBackbuffer->getBuffer(i)->getRenderTarget();
        renderTarget->removeListener(&mFaces[i]);
        renderTarget->removeAllViewports();

        Ogre::TextureManager::getSingleton().remove(mBackbuffer->getName());
    }
}
} }
