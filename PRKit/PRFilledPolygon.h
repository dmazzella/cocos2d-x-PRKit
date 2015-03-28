/*
 PRFilledPolygon.h
 
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

#ifndef PRFILLEDPOLYGON_H
#define PRFILLEDPOLYGON_H

#include "cocos2d.h"

using namespace cocos2d;

class FilledPolygon : public Node, public TextureProtocol
{
public:
    
    /**
     * Creates a FilledPolygon with a texture and a polygon in points, optionally determine bounding box.
     *
     * @note    If bound are determined the content size of the FilledPolygon are equal to size of bounding box,
     *          and anchor point (0,0) is in a origin of the bounding box.
     *
     * @param   texture          A Texture2D object whose texture will be applied to this sprite.
     * @param   texturePolygon   A vertices of polygon assigned the contents of texture.
     * @param   determineBounds  Whether or not determine bounds of polygon.
     * @return  An autoreleased FilledPolygon object.
     */
    static FilledPolygon* create(Texture2D* texture, const std::vector<Vec2> &texturePolygon, bool determineBounds = false);
    
    
    /**
     * Origin of bounding box of texture polygon.
     */
    virtual const Vec2& getBoundsOrigin() const { return _origin; }
    
    /**
     * Whether or not the FilledPolygon detrmines bounds of texture polygon.
     */
    virtual bool isDetermineBounds() const { return _determineBounds; }
    
    /**
     * Sets the texture polygon in points.
     * It will update the texture coordinates and the polygon vertices.
     */
    virtual void setTexturePolygon(const std::vector<Vec2> &texturePolygon, bool determineBounds = false);
    
    virtual void setTexture(Texture2D *texture) override;
    virtual Texture2D* getTexture() const override { return _texture; }
    
    virtual void updateColor() override;
    
    inline void setBlendFunc(const BlendFunc &blendFunc) override { _blendFunc = blendFunc; }
    inline const BlendFunc& getBlendFunc() const override { return _blendFunc; }
    
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;


CC_CONSTRUCTOR_ACCESS:

    FilledPolygon();
    virtual ~FilledPolygon();

    /**
     * Initializes a FilledPolygon with a texture and a polygon in points, optionally determine bounding box.
     *
     * @note    If bound are determined the content size of the FilledPolygon are equal to size of bounding box,
     *          and anchor point (0,0) is in a origin of the bounding box.
     *
     * @param   texture          A Texture2D object whose texture will be applied to this sprite.
     * @param   texturePolygon   A vertices of polygon assigned the contents of texture.
     * @param   determineBounds  Whether or not determine bounds of polygon.
     * @return  true if the sprite is initialized properly, false otherwise.
     */
    virtual bool init(Texture2D* texture, const std::vector<Vec2> &texturePolygon, bool determineBounds = false);


protected:

    Texture2D *_texture;
    BlendFunc _blendFunc;
    CustomCommand _customCommand;

    int _verticesCount;
    Vec2 *_positionTriangles;
    Vec2 *_textureTriangles;

    bool _determineBounds;
    Vec2 _origin;

    virtual void calculateTextureCoordinates();

    void onDraw(const Mat4 &transform, uint32_t flags);
};


#endif

