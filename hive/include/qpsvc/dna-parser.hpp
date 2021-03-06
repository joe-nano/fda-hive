/*
 *  ::718604!
 * 
 * Copyright(C) November 20, 2014 U.S. Food and Drug Administration
 * Authors: Dr. Vahan Simonyan (1), Dr. Raja Mazumder (2), et al
 * Affiliation: Food and Drug Administration (1), George Washington University (2)
 * 
 * All rights Reserved.
 * 
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#pragma once
#ifndef DnaParser_hpp
#define DnaParser_hpp

#include <ssci/bio.hpp>
#include <qpsvc/qpsvc.hpp>

class DnaParser: public sQPSvc
{
        typedef sQPSvc TParent;

    public:

        DnaParser(sQPride& qp, const char * file, const sHiveId & objId, const char * dstTypeName, const bool isVioseqlist = false, const udx maxChunkSize = ((udx) 8) * 1024 * 1024 * 1024, const char * dataType = 0, const char * userFilename = 0);
        virtual ~DnaParser();

        virtual const char* getSvcName() const
        {
            return "dna-parser";
        }

        void setFile(const char * file, ...) __attribute__((format(printf, 2, 3)));
        /**
         * To be used for reporting error for the user
         */
        void setUserFilename(const char * file, ...) __attribute__((format(printf, 2, 3)));
        void setObjId(const sHiveId & objId);
        void setMerge(bool isMerged); // chunks merged into one
        void setSingleFile(bool isSingle); // produce single file
        void setVioseqlist(bool isHiveseq);
        void setTreatAsTypeFile(const char *dataType);
        void setTypeName(const char * typeName);

        virtual udx split() const;

    protected:

        idx m_fileSize;
        udx m_maxChunkSize;

};

#endif // DnaParser_hpp
