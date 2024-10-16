#include "Framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer(CreateInfo const &createInfo)
{
    glGenFramebuffers(1, &m_FboID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);

    for (int i = 0; i < createInfo.attachements.size(); i++)
    {
        if (GL_COLOR_ATTACHMENT0 <= createInfo.attachements[i].attachement && createInfo.attachements[i].attachement <= GL_COLOR_ATTACHMENT31)
        {
            currentSizeX = createInfo.attachements[i].width;
            currentSizeY = createInfo.attachements[i].height;

            unsigned int m_Handle = 0;
            glGenTextures(1, &m_Handle);
            glBindTexture(GL_TEXTURE_2D, m_Handle);

            for (int j = 0; j < createInfo.attachements[i].chainDepth; j++)
            {
                glViewport(0, 0, currentSizeX, currentSizeY);

                glTexImage2D(GL_TEXTURE_2D, j, GL_RGB16F, currentSizeX, currentSizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                currentSizeX /= createInfo.attachements[i].mipSizeDownscale;
                currentSizeY /= createInfo.attachements[i].mipSizeDownscale;
            }

            glViewport(0, 0, createInfo.attachements[i].width, createInfo.attachements[i].height);
            glBindTexture(GL_TEXTURE_2D, 0);

            m_TexIDs.push_back(m_Handle);
            glFramebufferTexture2D(GL_FRAMEBUFFER, createInfo.attachements[i].attachement, GL_TEXTURE_2D, m_TexIDs.back(), 0);
            
            unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, attachments);
        }

        if (createInfo.attachements[i].attachement == GL_DEPTH_STENCIL_ATTACHMENT || createInfo.attachements[i].attachement == GL_DEPTH_ATTACHMENT)
        {
            unsigned int m_Handle = 0;
            glGenRenderbuffers(1, &m_Handle);
            glBindRenderbuffer(GL_RENDERBUFFER, m_Handle);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, createInfo.attachements[i].width, createInfo.attachements[i].height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, createInfo.attachements[i].attachement, GL_RENDERBUFFER, m_Handle);

            m_RboIDs.push_back(m_Handle);
        }
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Failed to create framebuffer!\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() noexcept
{
    glDeleteFramebuffers(1, &m_FboID);
    glDeleteRenderbuffers(m_RboIDs.size(), m_RboIDs.data());
    glDeleteTextures(m_TexIDs.size(), m_TexIDs.data());
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FboID);
}

void Framebuffer::bindTex(const int i) const
{
    if (i > m_TexIDs.size())
    {
        std::cerr << "Index I is greater than m_TexIDS\n";
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_TexIDs[i]);
}

void Framebuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   
}

void Framebuffer::changeRes(const int width, const int height, const int i) const
{
    this->bind();

    if (m_RboIDs.size() >= i)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, m_RboIDs[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    
    if (m_TexIDs.size() >= i)
    {
        this->bindTex(i);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    this->unbind();
}
