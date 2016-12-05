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

#include "CursorWin.h"

#include <android/graphics/Bitmap.h>

namespace android {
namespace view {
namespace appkit {

std::shared_ptr<Cursor> Cursor::systemCursor(Type cursorType)
{
    switch (cursorType) {
    case Type::Default:
        return CursorWin::defaultCursor();
    case Type::Retain:
        return CursorWin::retainCursor();
    case Type::Pointer:
    case Type::Cell:
    case Type::ContextMenu:
    case Type::Alias:
    case Type::Copy:
    case Type::None:
    case Type::Grab:
    case Type::Grabbing:
        return CursorWin::arrowCursor();
    case Type::Cross:
        return CursorWin::crossCursor();
    case Type::Hand:
        return CursorWin::handCursor();
    case Type::IBeam:
        return CursorWin::iBeamCursor();
    case Type::Wait:
        return CursorWin::waitCursor();
    case Type::Help:
        return CursorWin::helpCursor();
    case Type::Move:
        return CursorWin::moveCursor();
    case Type::MiddlePanning:
        return CursorWin::middlePanningCursor();
    case Type::EastResize:
        return CursorWin::eastResizeCursor();
    case Type::EastPanning:
        return CursorWin::eastPanningCursor();
    case Type::NorthResize:
        return CursorWin::northResizeCursor();
    case Type::NorthPanning:
        return CursorWin::northPanningCursor();
    case Type::NorthEastResize:
        return CursorWin::northEastResizeCursor();
    case Type::NorthEastPanning:
        return CursorWin::northEastPanningCursor();
    case Type::NorthWestResize:
        return CursorWin::northWestResizeCursor();
    case Type::NorthWestPanning:
        return CursorWin::northWestPanningCursor();
    case Type::SouthResize:
        return CursorWin::southResizeCursor();
    case Type::SouthPanning:
        return CursorWin::southPanningCursor();
    case Type::SouthEastResize:
        return CursorWin::southEastResizeCursor();
    case Type::SouthEastPanning:
        return CursorWin::southEastPanningCursor();
    case Type::SouthWestResize:
        return CursorWin::southWestResizeCursor();
    case Type::SouthWestPanning:
        return CursorWin::southWestPanningCursor();
    case Type::WestResize:
        return CursorWin::westResizeCursor();
    case Type::NorthSouthResize:
        return CursorWin::northSouthResizeCursor();
    case Type::EastWestResize:
        return CursorWin::eastWestResizeCursor();
    case Type::WestPanning:
        return CursorWin::westPanningCursor();
    case Type::NorthEastSouthWestResize:
        return CursorWin::northEastSouthWestResizeCursor();
    case Type::NorthWestSouthEastResize:
        return CursorWin::northWestSouthEastResizeCursor();
    case Type::ColumnResize:
        return CursorWin::columnResizeCursor();
    case Type::RowResize:
        return CursorWin::rowResizeCursor();
    case Type::VerticalText:
        return CursorWin::verticalTextCursor();
    case Type::Progress:
        return CursorWin::progressCursor();
    case Type::NoDrop:
    case Type::NotAllowed:
        return CursorWin::notAllowedCursor();
    case Type::ZoomIn:
        return CursorWin::zoomInCursor();
    case Type::ZoomOut:
        return CursorWin::zoomOutCursor();
    default:
        return CursorWin::arrowCursor();
    }

    return CursorWin::arrowCursor();
}

CursorWin::CursorWin(Type type, HCURSOR nativeCursor)
    : Cursor(type)
    , m_nativeCursor(nativeCursor)
{
}

CursorWin::~CursorWin()
{
    if (getType() == Type::Custom)
        ::DestroyCursor(m_nativeCursor);
}

std::shared_ptr<Cursor> CursorWin::defaultCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::Default, ::LoadCursorW(0, IDC_ARROW));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::retainCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::Retain, ::LoadCursorW(0, IDC_ARROW));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::arrowCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::Pointer, ::LoadCursorW(0, IDC_ARROW));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::crossCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::Cross, ::LoadCursorW(0, IDC_CROSS));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::handCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::Hand, ::LoadCursorW(0, IDC_HAND));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::iBeamCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::IBeam, ::LoadCursorW(0, IDC_IBEAM));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::waitCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::Wait, ::LoadCursorW(0, IDC_WAIT));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::helpCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::Help, ::LoadCursorW(0, IDC_HELP));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::moveCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::Move, ::LoadCursorW(0, IDC_SIZEALL));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::middlePanningCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::eastResizeCursor()
{
    return eastWestResizeCursor();
}

std::shared_ptr<Cursor> CursorWin::eastPanningCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::northResizeCursor()
{
    return northSouthResizeCursor();
}

std::shared_ptr<Cursor> CursorWin::northPanningCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::northEastResizeCursor()
{
    return northEastSouthWestResizeCursor();
}

std::shared_ptr<Cursor> CursorWin::northEastPanningCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::northWestResizeCursor()
{
    return northWestSouthEastResizeCursor();
}

std::shared_ptr<Cursor> CursorWin::northWestPanningCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::southResizeCursor()
{
    return northSouthResizeCursor();
}
std::shared_ptr<Cursor> CursorWin::southPanningCursor()
{
    // FIXME: need to Implement
    return CursorWin::arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::southEastResizeCursor()
{
    return northWestSouthEastResizeCursor();
}

std::shared_ptr<Cursor> CursorWin::southEastPanningCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::southWestResizeCursor()
{
    return northEastSouthWestResizeCursor();
}

std::shared_ptr<Cursor> CursorWin::southWestPanningCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::westResizeCursor()
{
    return eastWestResizeCursor();
}

std::shared_ptr<Cursor> CursorWin::northSouthResizeCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::NorthSouthResize, ::LoadCursorW(0, IDC_SIZENS));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::eastWestResizeCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::EastWestResize, ::LoadCursorW(0, IDC_SIZEWE));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::westPanningCursor()
{
    // FIXME: need to Implement
    return CursorWin::arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::northEastSouthWestResizeCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::NorthEastSouthWestResize, ::LoadCursorW(0, IDC_SIZENESW));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::northWestSouthEastResizeCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::NorthWestSouthEastResize, ::LoadCursorW(0, IDC_SIZENWSE));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::columnResizeCursor()
{
    // FIXME: need to Implement
    return eastWestResizeCursor();
}

std::shared_ptr<Cursor> CursorWin::rowResizeCursor()
{
    // FIXME: need to Implement
    return northSouthResizeCursor();
}

std::shared_ptr<Cursor> CursorWin::verticalTextCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::progressCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::Progress, ::LoadCursorW(0, IDC_APPSTARTING));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::notAllowedCursor()
{
    static auto cursor = std::make_shared<CursorWin>(Type::NotAllowed, ::LoadCursorW(0, IDC_NO));
    return cursor;
}

std::shared_ptr<Cursor> CursorWin::zoomInCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

std::shared_ptr<Cursor> CursorWin::zoomOutCursor()
{
    // FIXME: need to Implement
    return arrowCursor();
}

} // namespace appkit
} // namespace view
} // namespace android
