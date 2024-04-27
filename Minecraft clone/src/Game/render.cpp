#include "render.h"
#include "Entity/player.h"
#include "Gui/gui.h"

void Renderer::Startup()
{
	// Frame stuff
	glex::TextureInfo tf;
	tf.width = 800;
	tf.height = 600;
	tf.format = glex::Texture::RGBA;
	s_frameTexture = glex::Gl::CreateTexture(nullptr, tf);
	s_frameRenderBuffer = glex::Gl::CreateRenderBuffer(glex::RenderBuffer::Depth24, 800, 600);
	s_frameBuffer = glex::Gl::CreateFrameBuffer(800.0f, 600.0f);
	s_frameBuffer.AttachColor(0, s_frameTexture);
	s_frameBuffer.AttachDepth(s_frameRenderBuffer);
	// Chunk stuff
	s_chunkVertexLayout = glex::Gl::CreateVertexLayout({ { glex::VertexLayout::Float, 3 }, { glex::VertexLayout::Float, 2 }, { glex::VertexLayout::Uint, 1 } });
	s_chunkShader = glex::Gl::CreateShader(glex::File::ReadAllText("./src/Shaders/chunk.glsl").c_str());
	tf.format = glex::Texture::Auto;
	tf.filterMag = glex::Texture::Nearest;
	s_chunkTexture = glex::Gl::CreateTexture("./res/block.png", tf);
	s_chunkMaterial = glex::Gl::CreateUniformBuffer(24);
	glex::Gl::BindUniformBuffer(s_chunkMaterial, glex::UniformBuffer::MaterialBlock); // Bind it once and for all.
	// Outline stuff
	static float wireVertices[] = {
		-0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,
	};
	static uint32_t wireIndices[] = {
		1, 5, 6,
		6, 2, 1,
		2, 6, 7,
		7, 3, 2,
		3, 7, 4,
		4, 0, 3,
		0, 4, 5,
		5, 1, 0,
		0, 1, 2,
		2, 3, 0,
		6, 5, 4,
		4, 7, 6
	};
	s_wireVertexLayout = glex::Gl::CreateVertexLayout({ { glex::VertexLayout::Float, 3 } });
	s_wireVertexBuffer = glex::Gl::CreateVertexBuffer(wireVertices, sizeof(wireVertices));
	s_wireIndexBuffer = glex::Gl::CreateIndexBuffer(wireIndices, sizeof(wireIndices));
	tf.filterMag = glex::Texture::Linear;
	s_starTexture = glex::Gl::CreateTexture("./res/star.jpg", tf);
	s_wireShader = glex::Gl::CreateShader(glex::File::ReadAllText("./src/Shaders/wire.glsl").c_str());
	s_skyboxShader = glex::Gl::CreateShader(glex::File::ReadAllText("./src/Shaders/sky.glsl").c_str());
	glex::Renderer::SetLineWidth(2.0f);
	// Block stuff
	s_blockVertexLayout = glex::Gl::CreateVertexLayout({ { glex::VertexLayout::Float, 3 }, { glex::VertexLayout::Float, 2 }, { glex::VertexLayout::Float, 3 } });
	s_blockVertexBuffer = glex::Gl::CreateVertexBuffer(Config::BlockVertexBatch * 8 * 4);
	s_blockIndexBuffer = glex::Gl::CreateIndexBuffer(Config::BlockIndexBatch * 4);
	s_blockShader = glex::Gl::CreateShader(glex::File::ReadAllText("./src/Shaders/block.glsl").c_str());
	// GUI stuff
	tf.filterMin = glex::Texture::Linear;
	s_fontTexture = glex::Gl::CreateTexture("./res/book.png", tf);
	s_font = glex::Gl::CreateFont(s_fontTexture, "./res/book.fnt");
	tf.filterMin = glex::Texture::Nearest;
	s_widgets = glex::Gl::CreateTexture("./res/controls.png", tf);
	s_title = glex::Gl::CreateTexture("./res/title.png", tf);
	s_background = glex::Gl::CreateTexture("./res/pink bg.jpg", tf);
	// Startup
	s_camera.Perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, Config::RenderDistance() * Config::ChunkSize);
}

void Renderer::Shutdown()
{
	// Frame buffer stuff
	glex::Gl::DestroyTexture(s_frameTexture);
	glex::Gl::DestroyRenderBuffer(s_frameRenderBuffer);
	glex::Gl::DestroyFrameBuffer(s_frameBuffer);
	// Chunk stuff
	glex::Gl::DestroyVertexLayout(s_chunkVertexLayout);
	glex::Gl::DestroyShader(s_chunkShader);
	glex::Gl::DestroyTexture(s_chunkTexture);
	glex::Gl::DestroyUniformBuffer(s_chunkMaterial);
	// Outline stuff
	glex::Gl::DestroyVertexLayout(s_wireVertexLayout);
	glex::Gl::DestroyVertexBuffer(s_wireVertexBuffer);
	glex::Gl::DestroyIndexBuffer(s_wireIndexBuffer);
	glex::Gl::DestroyTexture(s_starTexture);
	glex::Gl::DestroyShader(s_wireShader);
	glex::Gl::DestroyShader(s_skyboxShader);
	// Block stuff
	glex::Gl::DestroyVertexLayout(s_blockVertexLayout);
	glex::Gl::DestroyVertexBuffer(s_blockVertexBuffer);
	glex::Gl::DestroyIndexBuffer(s_blockIndexBuffer);
	glex::Gl::DestroyShader(s_blockShader);
	// GUI stuff
	glex::Gl::DestroyTexture(s_fontTexture);
	glex::Gl::DestroyTexture(s_widgets);
	glex::Gl::DestroyTexture(s_title);
	glex::Gl::DestroyTexture(s_background);
}

void Renderer::OnSetRenderDistance()
{
	Renderer::SetFog(glm::vec4(glm::vec3(Player::WorldIn().SkyColor()), Config::RenderDistance() * Config::ChunkSize));
	s_camera.Perspective(glm::radians(45.0f), s_frameBuffer.Width() / s_frameBuffer.Height(), 0.1f, Config::RenderDistance() * Config::ChunkSize);
}

void Renderer::OnResize(int width, int height)
{
	float w = width, h = height;
	s_frameTexture.Resize(width, height);
	s_frameRenderBuffer.Resize(width, height);
	s_frameBuffer.Resize(w, h);
	glex::Renderer::ViewPort(0, 0, width, height);
	glex::Renderer::SetScreenResolution(w, h);
	s_camera.Perspective(glm::radians(45.0f), w / h, 0.1f, Config::RenderDistance() * Config::ChunkSize);
	Gui::OnResize(w, h); // Reset GUI resolution
}

void Renderer::RenderVisibleChunks()
{
	glex::Gl::BindVertexLayout(s_chunkVertexLayout);
	glex::Gl::BindShader(s_chunkShader);
	glex::Gl::BindTexture(s_chunkTexture, 0);
	glex::Renderer::EnableDepthTest();
	glex::Renderer::DisableBlending();
	for (auto& [pos, chunk] : Player::VisibleChunks())
	{
		chunk->Upload();
		chunk->DrawSolid();
	}
	glex::Renderer::DisableDepthTest();
	glex::Renderer::EnableBlending();
	for (auto& [pos, chunk] : Player::VisibleChunks())
		chunk->DrawTransparent();
}

void Renderer::RenderBlockOutline()
{
	World::Ray const& ray = World::RaycastResult();
	if (ray.blockHit)
	{
		glm::mat4 modelMat = glm::mat4(1.0f);
		if (ray.block.IsFull())
			modelMat = glm::translate(modelMat, glm::vec3(ray.blockPos));
		else
		{
			glm::vec3 scale = glm::abs(ray.bbox->upperRightFront - ray.bbox->lowerLeftBack);
			glm::vec3 offset = (ray.bbox->upperRightFront + ray.bbox->lowerLeftBack) / 2.0f;
			modelMat = glm::translate(modelMat, glm::vec3(ray.blockPos) + offset);
			modelMat = glm::scale(modelMat, scale);
		}
		glex::Renderer::SetModelMatrix(modelMat);
		glex::Renderer::EnableDepthTest();
		glex::Renderer::DisableBlending();
		glex::Gl::BindVertexLayout(s_wireVertexLayout);
		glex::Gl::BindVertexBuffer(s_wireVertexBuffer);
		glex::Gl::BindIndexBuffer(s_wireIndexBuffer);
		glex::Gl::BindShader(s_wireShader);
		glex::Gl::Draw();
	}
}

void Renderer::RenderSky()
{
#ifdef _DEBUG
	if (glex::Input::Pressed(glex::Input::R))
	{
		glex::Gl::DestroyShader(s_skyboxShader);
		s_skyboxShader = glex::Gl::CreateShader(glex::File::ReadAllText("./src/Shaders/sky.glsl").c_str());
		std::cout << "Skybox shader reloaded.\n";
	}
#endif
	glex::Renderer::EnableDepthTest();
	glex::Renderer::DisableBlending();
	glex::Renderer::DepthLessOrEqual();
	glex::Renderer::SetViewMatix(glm::mat4(glm::mat3(s_camera.ViewMat())));
	glex::Gl::BindTexture(s_starTexture, 0);
	glex::Gl::BindVertexLayout(s_wireVertexLayout);
	glex::Gl::BindVertexBuffer(s_wireVertexBuffer);
	glex::Gl::BindIndexBuffer(s_wireIndexBuffer);
	glex::Gl::BindShader(s_skyboxShader);
	glex::Gl::Draw();
	glex::Renderer::DepthLess();
}

void Renderer::RenderWorld()
{
	glex::Renderer::SetProjectionMatrix(s_camera.ProjMat());
	glex::Renderer::SetViewMatix(s_camera.ViewMat());
	glex::Gl::BindFrameBuffer(s_frameBuffer);
	glex::Renderer::ClearDepth();
	glex::Renderer::EnableFaceCulling();
	RenderVisibleChunks();
	RenderBlockOutline();
	glex::Renderer::DisableFaceCulling();
	RenderSky();
	glex::Gl::BindFrameBuffer(glex::Context::DefaultFrameBuffer());
	glex::Renderer::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	glex::Renderer::UIStart();
	glex::Renderer::DrawQuad(glm::vec4(0.0f, 0.0f, s_frameBuffer.Width(), s_frameBuffer.Height()), s_frameTexture);
}

void Renderer::RenderHotbar()
{
	constexpr float outerSize = 82.0f, innerSize = 72.0f, padding = 5.0f, bottom = 24.0f;
	constexpr float fullWidth = outerSize * Config::HotbarSlots;
	static bool showHud = true;
	showHud ^= glex::Input::Pressed(glex::Input::F1);
	if (showHud)
	{
		float scrWidth = Renderer::s_frameBuffer.Width(), scrHeight = Renderer::s_frameBuffer.Height();
		float scale = Config::GuiScale();
		float realFullWidth = fullWidth * scale;
		float realSize = outerSize * scale;
		float left = (scrWidth - realFullWidth) / 2.0f;
		float up = scrHeight - (bottom + outerSize) * scale;
		float activeLeft = left + realSize * Player::ItemIndex();
		glex::Renderer::DrawQuad(glm::vec4(left, up, left + realFullWidth, up + realSize), Config::LightColor);
		glex::Renderer::DrawQuad(glm::vec4(activeLeft, up, activeLeft + realSize, up + realSize), Config::DeepColor);
		float realPadding = padding * scale;
		float realInnerSize = realSize - realPadding - realPadding;
		float quadLeft = left;
		Gui::StartItemDrawing();
		for (uint32_t i = 0; i < Config::HotbarSlots; i++)
		{
			float innerLeft = quadLeft + realPadding;
			Item const& item = Player::GetItem(i);
			if (item.id != Block::Air)
				Gui::DrawItem(glm::vec3(innerLeft, up + realPadding, realInnerSize), item);
			quadLeft += realSize;
		}
		Gui::ItemDrawingDone();
		if (s_fadeTime > 0.0f)
		{
			glex::Renderer::DrawText(glm::vec4(0.0f, up - 43.0f * scale, scrWidth, scrHeight),
				Renderer::s_font, s_itemName, 32.0f * scale, 32.0f * Config::Flatness * scale, Config::TextSpace, Config::LineSpace, 0.0f,
				glm::vec4(1.0f, 1.0f, 1.0f, s_fadeTime > 1000.0f ? 1.0f : s_fadeTime / 1000.0f), 0);
			s_fadeTime -= glex::Time::DeltaTime();
		}
		float crossHairSize = scale * 32.0f;
		float crossLeft = (scrWidth - crossHairSize) / 2.0f;
		float crossUp = (scrHeight - crossHairSize) / 2.0f;
		glex::Renderer::DrawQuad(glm::vec4(crossLeft, crossUp, crossLeft + crossHairSize, crossUp + crossHairSize), s_widgets, s_crosshair);
	}
}