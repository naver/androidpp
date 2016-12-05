/*
 * Copyright (C) 2016 Naver Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <android/view/appkit/Cursor.h>

#include <windows.h>

namespace android {
namespace view {
namespace appkit {

class CursorWin : public Cursor {
public:
    friend Cursor;
    CursorWin(Type, HCURSOR);
    ~CursorWin();

    HCURSOR nativeCursor() const { return m_nativeCursor; }

private:
    HCURSOR m_nativeCursor;

    static std::shared_ptr<Cursor> defaultCursor();
    static std::shared_ptr<Cursor> retainCursor();
    static std::shared_ptr<Cursor> arrowCursor();
    static std::shared_ptr<Cursor> crossCursor();
    static std::shared_ptr<Cursor> handCursor();
    static std::shared_ptr<Cursor> iBeamCursor();
    static std::shared_ptr<Cursor> waitCursor();
    static std::shared_ptr<Cursor> helpCursor();
    static std::shared_ptr<Cursor> moveCursor();
    static std::shared_ptr<Cursor> middlePanningCursor();
    static std::shared_ptr<Cursor> eastResizeCursor();
    static std::shared_ptr<Cursor> eastPanningCursor();
    static std::shared_ptr<Cursor> northResizeCursor();
    static std::shared_ptr<Cursor> northPanningCursor();
    static std::shared_ptr<Cursor> northEastResizeCursor();
    static std::shared_ptr<Cursor> northEastPanningCursor();
    static std::shared_ptr<Cursor> northWestResizeCursor();
    static std::shared_ptr<Cursor> northWestPanningCursor();
    static std::shared_ptr<Cursor> southResizeCursor();
    static std::shared_ptr<Cursor> southPanningCursor();
    static std::shared_ptr<Cursor> southEastResizeCursor();
    static std::shared_ptr<Cursor> southEastPanningCursor();
    static std::shared_ptr<Cursor> southWestResizeCursor();
    static std::shared_ptr<Cursor> southWestPanningCursor();
    static std::shared_ptr<Cursor> westResizeCursor();
    static std::shared_ptr<Cursor> northSouthResizeCursor();
    static std::shared_ptr<Cursor> eastWestResizeCursor();
    static std::shared_ptr<Cursor> westPanningCursor();
    static std::shared_ptr<Cursor> northEastSouthWestResizeCursor();
    static std::shared_ptr<Cursor> northWestSouthEastResizeCursor();
    static std::shared_ptr<Cursor> columnResizeCursor();
    static std::shared_ptr<Cursor> rowResizeCursor();
    static std::shared_ptr<Cursor> verticalTextCursor();
    static std::shared_ptr<Cursor> progressCursor();
    static std::shared_ptr<Cursor> notAllowedCursor();
    static std::shared_ptr<Cursor> zoomInCursor();
    static std::shared_ptr<Cursor> zoomOutCursor();
};

} // namespace appkit
} // namespace view
} // namespace android
