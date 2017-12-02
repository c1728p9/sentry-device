/* mbed Microcontroller Library
 * Copyright (c) 2017-2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "FileIterator.h"

FileIterator::FileIterator(FileSystem *fs, const char *path): _fs(fs), _error(false), _path(path)
{
    int ret = _dir.open(fs, path);
    if (ret) {
        printf("Error opening generator directory %s\n", path);
        _error = true;
    }
}

const char *FileIterator::next()
{
    if (_error) {
        return NULL;
    }
    const char *str = _next();

    // Reopen the directory
    if (str == NULL) {
        _dir.close();
        int ret = _dir.open(_fs, _path.c_str());
        if (ret) {
            _error = true;
            return NULL;
        }
        str = _next();
    }
    return str;
}



const char *FileIterator::_next()
{
    if (_error) {
        return NULL;
    }

    struct dirent entry;
    while (_dir.read(&entry) > 0) {
        if (entry.d_type == DT_REG) {
            _filepath = _path;
            _filepath += "/";
            _filepath += entry.d_name;
            return _filepath.c_str();
        }
    }
    _dir.close();
    return NULL;

}
