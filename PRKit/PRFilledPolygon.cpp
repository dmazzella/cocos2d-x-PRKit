/*
 PRFilledPolygon.cpp
 
 PRKit:  Precognitive Research additions to Cocos2D.  http://cocos2d-iphone.org
 Contact us if you like it:  http://precognitiveresearch.com
 
 Created by Andy Sinesio on 6/25/10.
 Copyright 2011 Precognitive Research, LLC. All rights reserved.
 
 This class fills a polygon, described by an array of vertices, with a texture.
 
 Translated in C++ for Cocos2d-X by Damiano Mazzella on 19/03/2012
 Updated by Petro Shmigelskyi on 03/28/2015
 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "PRFilledPolygon.h"
#include "triangulate.h"


FilledPolygon* FilledPolygon::create(Texture2D* texture, const std::vector<Vec2> &texturePolygon, bool determineBounds)
{
    FilledPolygon *pRet = new FilledPolygon();
    if(pRet && pRet->init(texture, texturePolygon, determineBounds))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool FilledPolygon::init(Texture2D* texture, const std::vector<Vec2> &texturePolygon, bool determineBounds)
{
    GLProgram *glProgram = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_U_COLOR);
    setGLProgramState(GLProgramState::create(glProgram));
    
    setTexture(texture);
    setTexturePolygon(texturePolygon, determineBounds);
    updateColor();
    
	return true;
}


FilledPolygon::FilledPolygon()
: _verticesCount(0)
, _texture(nullptr)
, _positionTriangles(nullptr)
, _textureTriangles(nullptr)
, _origin(Vec2::ZERO)
{}

FilledPolygon::~FilledPolygon()
{
    CC_SAFE_RELEASE_NULL(_texture);
    CC_SAFE_DELETE_ARRAY(_positionTriangles);
    CC_SAFE_DELETE_ARRAY(_textureTriangles);
}


void FilledPolygon::setTexturePolygon(const std::vector<Vec2> &texturePolygon, bool determineBounds)
{
    CC_SAFE_DELETE_ARRAY(_positionTriangles);
    CC_SAFE_DELETE_ARRAY(_textureTriangles);
    
    _determineBounds = determineBounds;
    
    // triangulate polygon using Ratcliff's triangulator
    std::vector<Vec2> triangulatedVertices;
    Triangulate::Process(texturePolygon, triangulatedVertices);
    
    _verticesCount = (int)triangulatedVertices.size();
    _positionTriangles = new Vec2[_verticesCount];
    _textureTriangles = new Vec2[_verticesCount];
    
    // determine polygon bounds
    if (_determineBounds) {
        _origin = Vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Vec2 _maxBound = Vec2::ZERO;
        
        for (const Vec2& v : triangulatedVertices){
            if(v.x < _origin.x) _origin.x = v.x;
            if(v.y < _origin.y) _origin.y = v.y;
            if(v.x > _maxBound.x) _maxBound.x = v.x;
            if(v.y > _maxBound.y) _maxBound.y = v.y;
        }
        
        setContentSize(Size(_maxBound - _origin));
        
        for (int i = 0; i < _verticesCount; i++) {
            _positionTriangles[i] = triangulatedVertices.at(i) - _origin;
        }
        
    } else {
        for (int i = 0; i < _verticesCount; i++) {
            _positionTriangles[i] = triangulatedVertices.at(i);
        }
    }
    
    calculateTextureCoordinates();
    
    // set parameters to shader attributes
    getGLProgramState()->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_POSITION, 2, GL_FLOAT, GL_FALSE, 0, _positionTriangles);
    getGLProgramState()->setVertexAttribPointer(GLProgram::ATTRIBUTE_NAME_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, _textureTriangles);
}

void FilledPolygon::calculateTextureCoordinates()
{
    if (_determineBounds) {
        for (int j = 0; j < _verticesCount; j++) {
            _textureTriangles[j].x = (_positionTriangles[j].x + _origin.x) * 1.0f / _texture->getPixelsWide() * CC_CONTENT_SCALE_FACTOR();
            _textureTriangles[j].y = 1.0f - (_positionTriangles[j].y + _origin.y) * 1.0f / _texture->getPixelsHigh() * CC_CONTENT_SCALE_FACTOR();
        }
    } else {
        for (int j = 0; j < _verticesCount; j++) {
            _textureTriangles[j].x = _positionTriangles[j].x * 1.0f / _texture->getPixelsWide() * CC_CONTENT_SCALE_FACTOR();
            _textureTriangles[j].y = 1.0f - _positionTriangles[j].y * 1.0f / _texture->getPixelsHigh() * CC_CONTENT_SCALE_FACTOR();
        }
    }
}

void FilledPolygon::setTexture(Texture2D* texture)
{
    if( _texture != texture )
    {
        CCASSERT(dynamic_cast<Texture2D*>(texture), "setTexture expects a Texture2D. Invalid argument");
        
        CC_SAFE_RELEASE(_texture);
        CC_SAFE_RETAIN(texture);
        _texture = texture;
        
        // set repeat mode for POT-size texture
        if(_texture->getPixelsWide() == ccNextPOT(_texture->getPixelsWide()) && _texture->getPixelsHigh() == ccNextPOT(_texture->getPixelsHigh())){
            _texture->setTexParameters({ GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT });
        }
        
        // update blend func
        if ( !_texture || !_texture->hasPremultipliedAlpha() ) {
            _blendFunc.src = GL_SRC_ALPHA;
            _blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
        } else {
            _blendFunc.src = CC_BLEND_SRC;
            _blendFunc.dst = CC_BLEND_DST;
        }
        
        // update texture triangles
        if (_textureTriangles != nullptr) {
            calculateTextureCoordinates();
        }
    }
}

void FilledPolygon::updateColor()
{
    // set parameters to shader uniform
    getGLProgramState()->setUniformVec4("u_color", Vec4(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity)/255.0f);
}


void FilledPolygon::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    if(_verticesCount <= 1)
        return;
    
    _customCommand.init(_globalZOrder);
    _customCommand.func = CC_CALLBACK_0(FilledPolygon::onDraw, this, transform, flags);
    renderer->addCommand(&_customCommand);
}

void FilledPolygon::onDraw(const Mat4 &transform, uint32_t flags)
{
    GL::bindTexture2D(_texture->getName());
    GL::blendFunc(_blendFunc.src, _blendFunc.dst);
    
    getGLProgramState()->apply(transform);
    
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)_verticesCount);
    
    CC_INCREMENT_GL_DRAWS(1);
    CHECK_GL_ERROR_DEBUG();
}
