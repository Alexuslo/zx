#include "common.h"
#include "App.h"
//#include "oxygine\core\oxygine.h"


// extern "C" FILE* __cdecl __iob_func()
// {
// 	struct _iobuf_VS2012 { // ...\Microsoft Visual Studio 11.0\VC\include\stdio.h #56
// 		char* _ptr;
// 		int   _cnt;
// 		char* _base;
// 		int   _flag;
// 		int   _file;
// 		int   _charbuf;
// 		int   _bufsiz;
// 		char* _tmpfname;
// 	};
// 	// VS2015 has only FILE = struct {void*}
// 	int const count = sizeof(_iobuf_VS2012) / sizeof(FILE);
// 	// stdout
// 	//return (FILE*)(&(__acrt_iob_func(1)->_Placeholder) - count);
// 	// stderr
// 	return (FILE*)(&(__acrt_iob_func(2)->_Placeholder) - 2 * count);
// }

using namespace oxygine;

// spNativeTexture RTnew;
// spNativeTexture RTprevious;
// Color color;
// spSprite RTSprite;

int mainloop()
{
	bool bIsDone = core::update();
	if (!bIsDone)
	{
//		RTSprite->update(UpdateState());

		CApp::Update();
	}

	if (core::beginRendering())
    {

//		SDL_GL_SetSwapInterval(1);
        Color clearColor(32, 32, 32, 255);
        Rect viewport(Point(0, 0), core::getDisplaySize());
// 		
// 		STDRenderer &renderer = *STDRenderer::instance;
// 		IVideoDriver *driver = IVideoDriver::instance;
// 
// 		renderer.initCoordinateSystem(viewport.getWidth(), viewport.getHeight(), true);
//  		RTprevious = driver->getRenderTarget();
// 		driver->setRenderTarget(RTnew);
// 		driver->setViewport(viewport);
// 
// 		getStage()->render(clearColor, viewport);
// 		
// 		driver->setRenderTarget(RTprevious);
// 		driver->setViewport(viewport);
// 
// 		if (&clearColor)
// 			driver->clear(clearColor);

		getStage()->render(clearColor, viewport);

// 		Matrix proj;
// 		Matrix::orthoLH(proj, (float)viewport.getWidth(), (float)viewport.getHeight(), 0.2f, 10000);
// 		Matrix view = makeViewMatrix(viewport.getWidth(), viewport.getHeight(), true);
// 		Matrix vp = view * proj;
// 
// 		STDRenderer::instance->setViewProj(vp);
// 
// 		RenderState rs;
// 		Point ds = core::getDisplaySize();
// 
// 		RectF clip(0.0f, 0.0f, (float)ds.x, (float)ds.y);
// 		rs.clip = &clip;
// 
// 		static float rotation = 0.0f;
// 		rotation += 0.001f;
// 
// 		RTSprite->setRotation(rotation);
// 
// 
// 		RTSprite->render(rs);
// 				
// 		STDRenderer::getCurrent()->flush();
// 		Material::null->apply();
			   		 
		core::swapDisplayBuffers();
	}

	return bIsDone ? 1 : 0;
}

// Application entry point
void run()
{
	CApp::Init();

// 	Point size = core::getDisplaySize();
// 
// 	RTnew = IVideoDriver::instance->createTexture();
// 	RTnew->init((int)size.x, (int)size.y, TF_R8G8B8A8, true);
// 
// 	RTSprite = new Sprite;
// 	//RTSprite->attachTo(getStage());
// 
// 	AnimationFrame frame;
// 	Diffuse df;
// 	df.base = RTnew;
// 
// 	Vector2 FrameSize(size);
// 	RectF SrcRect(0.0f, 0.0f, FrameSize.x / static_cast<float>(RTnew->getWidth()), FrameSize.y / static_cast<float>(RTnew->getHeight()));
// 	RectF DestRect(Vector2(0.0f, 0.0f), FrameSize);
// 
// 	frame.init(nullptr, df, SrcRect, DestRect, FrameSize);
// 
// 	RTSprite->setAnimFrame(frame);
// 	//RTSprite->setBlendMode(blend_disabled);
// 	
// 	//RTSprite->setResAnim(g_pApp->getTexture("back"));
// 	RTSprite->setAnchor(0.5f, 0.5f);
// 	RTSprite->setAlpha(100);
// 	RTSprite->setTouchEnabled(false);
//  	RTSprite->setSize(getStage()->getSize());
//  	RTSprite->setPosition(getStage()->getSize() / 2);
//  	RTSprite->setPriority(31000);
// 	RTSprite->addTween(Sprite::TweenRotation(2 * PI), 5000, -1);
// 	RTSprite->setScale(0.75f, 0.75f);
	
#ifdef EMSCRIPTEN
    //If you build for Emscripten, mainloop is called automatically and shouldn't be called here. See emscripten_set_main_loop in the EMSCRIPTEN section below
    return;
#endif

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    //On iPhone mainloop is called automatically by CADisplayLink, see int main() below
    return;
#endif

	while (!mainloop());
    //renderer.cleanup();
	CApp::Destroy();
}

#ifdef OXYGINE_SDL

#include "SDL_main.h"
#include "SDL.h"

extern "C"
{
    void one(void* param) { mainloop(); }
    void oneEmsc() { mainloop(); }

    int main(int argc, char* argv[])
    {
		run();

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        // If parameter 2 is set to 1, refresh rate will be 60 fps, 2 - 30 fps, 3 - 15 fps.
        SDL_iPhoneSetAnimationCallback(core::getWindow(), 1, one, nullptr);
#endif

#if EMSCRIPTEN
        emscripten_set_main_loop(oneEmsc, 0, 0);
#endif
        return 0;
    }
};
#endif