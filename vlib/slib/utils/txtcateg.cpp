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
#include <slib/utils/txt.hpp>
#include <slib/std/string.hpp>

using namespace slib;

idx sText::categoryListParseCsv(sTabular * tbl, sVec < idx > * rowSet, sDic < sDic < sVec < idx > >  > * lcolset , sDic < idx > * ids, sVec < idx > * colsToAccumulateForCategorization, const char * empty, bool totalset)
{

    if(rowSet && !rowSet->dim())rowSet=0;
    idx irow=0, icol=0;//, ic=0, ir=0;
    idx spcnum=sNotIdx;
    sVec < sVec < idx >  > numCatVec;

    sStr t;
    char * ppcl;

    idx iColEnd=colsToAccumulateForCategorization ? colsToAccumulateForCategorization->dim() : tbl->cols();
    for ( idx iCol=0; iCol<iColEnd; ++iCol) {
        icol = colsToAccumulateForCategorization ? (*colsToAccumulateForCategorization)[iCol] : iCol;

        t.cut(0);tbl->printTopHeader(t,icol);
        ppcl=t.ptr();

        numCatVec.add();
        char * numcats=strchr(ppcl,':');
        if( numcats ) {
            sString::scanRangeSet(numcats+1,0,&numCatVec[iCol],0,0,0);
            *numcats=0;
        }
        lcolset->set(ppcl);
    }


    idx cntRows=rowSet? rowSet->dim(): tbl->rows();

    for ( irow=0; irow<cntRows; ++irow) {
        idx iRow=rowSet? (*rowSet)[irow]: irow;
        spcnum=iRow;
        if(ids) {
            t.cut(0);tbl->printLeftHeader(t,iRow);
            ppcl=t.ptr();

            if( !ids->get(ppcl,0,&spcnum)){ // retrieve numeral of the sample from provided ID array
                spcnum=sNotIdx;
                continue; // we must then have this id in id list
            }
        }

        for ( idx iCol=0; iCol<iColEnd; ++iCol) {
            icol = colsToAccumulateForCategorization ? (*colsToAccumulateForCategorization)[iCol] : iCol;

            t.cut(0);tbl->printCell(t,iRow,icol);
            ppcl=t.ptr();

            sDic < sVec < idx > > * pcolset=lcolset->ptr(iCol);
            if(!empty || strcmp(ppcl,empty)!=0 ) {
                sVec <idx > * cur=0;

                if(numCatVec[iCol].dim()>0){ // numerical vector needs to determine the range
                    idx irg;
                    real val=0;sscanf(ppcl,"%lf",&val);
                    for( irg=0; irg<numCatVec[iCol].dim() && val>(real)numCatVec[iCol][irg] ; ++irg) ;
                    sStr l;
                    if(irg==0)l.printf("<%" DEC,numCatVec[iCol][0]);
                    else if(irg==numCatVec[iCol].dim() )l.printf(">%" DEC,numCatVec[iCol][irg-1]);
                    else l.printf("]%" DEC "-%" DEC "]",numCatVec[iCol][irg-1],numCatVec[iCol][irg]);
                    cur=pcolset->set(l.ptr());
                }else {
                    cur=pcolset->set(ppcl);
                }
                if(cur)cur->vadd(1,spcnum);
            }

        }
    }
    if(totalset && ids){
        sDic < sVec < idx > > * pcolset=lcolset->set("_all_");
        sVec < idx > * pcolsetset=pcolset->set("_all_");
        for ( idx ii=0; ii<ids->dim(); ++ii) *pcolsetset->add()=ii;
    }
    return lcolset->dim();
}

idx sText::categoryListParseCsv(sStr * flbuf , const char * src, idx , sDic < sDic < sVec < idx > >  > * lcolset , sDic < idx > * ids, const char * empty, bool totalset, bool supportQuote) // len
{
    //if(!src || !len)return 0;
    if(!src)return 0;

    sString::searchAndReplaceSymbols(flbuf,src, sLen(src), sString_symbolsEndline,",,",0,true,true,false);
    flbuf->add0(4);
    sString::searchAndReplaceSymbols(flbuf->ptr(),flbuf->length(), ",;",0,0,true,false,true);
    const char* fp0=flbuf->ptr();
    
    const char * prow, * pcol;
    idx irow=0, icol=0;//, ic=0, ir=0;
    idx spcnum=sNotIdx;
    sVec < sVec < idx >  > numCatVec;
                
    for ( irow=0, prow=fp0; prow; prow=sString::next00(prow) , ++irow) { 
        spcnum=sNotIdx;
                
        for ( icol=0, pcol=prow; pcol; pcol=sString::next00(pcol) , ++icol) { 
            sStr t;
            if( icol > 0 ) {
                // columns 1+ are numeric, strip whitespace
                sString::cleanEnds(&t,pcol,0,sString_symbolsBlank,true);
            } else {
                // column 0 is sample name, whitespace is significant (must exactly match name in measurables matrix)
                t.addString(pcol);
            }
            char * ppcl=t.ptr();
            idx ppclen=sLen(ppcl);
            if(supportQuote) {
                char quote=*ppcl;
                if((quote=='\"' || quote=='\'' ) && ppcl[ppclen-1]==quote)
                    {++ppcl;ppclen-=2;}
            }
            
            if(irow==0 ){ 
                numCatVec.add(); 
                if(icol>0) {
                    char * numcats=strchr(ppcl,':');
                    if( numcats ) { 
                        sString::scanRangeSet(numcats+1,0,&numCatVec[icol],0,0,0);
                        *numcats=0;
                    }
                    lcolset->set(ppcl);
                }
            }
            else { 
                if( icol==0 ) {
                    spcnum=irow-1;
                    if(irow>0 && ids) {
                        if( !ids->get(ppcl,ppclen,&spcnum)){ // retrieve numeral of the sample from provided ID array
                            spcnum=sNotIdx;
                            continue; // we must then have this id in id list
                        }
                    }
                }
                else if(spcnum!=sNotIdx) { 
                    sDic < sVec < idx > > * pcolset=lcolset->ptr(icol-1);
                    if(!empty || strncmp(ppcl,empty,ppclen)!=0 ) {
                        sVec <idx > * cur=0;

                        if(numCatVec[icol].dim()>0){ // numerical vector needs to determine the range
                            idx irg;
                            real val=0;sscanf(ppcl,"%lf",&val);
                            for( irg=0; irg<numCatVec[icol].dim() && val>(real)numCatVec[icol][irg] ; ++irg) ;
                            sStr l;
                            if(irg==0)l.printf("<%" DEC,numCatVec[icol][0]);
                            else if(irg==numCatVec[icol].dim() )l.printf(">%" DEC,numCatVec[icol][irg-1]);
                            else l.printf("]%" DEC "-%" DEC "]",numCatVec[icol][irg-1],numCatVec[icol][irg]);
                            cur=pcolset->set(l.ptr());
                        }else {
                            cur=pcolset->set(ppcl,ppclen);
                        }
                        if(cur)cur->vadd(1,spcnum);
                    }
                }
            }

            prow=pcol+sLen(pcol)+1;
        }
    }
    if(totalset && ids){
        sDic < sVec < idx > > * pcolset=lcolset->set("_all_");
        sVec < idx > * pcolsetset=pcolset->set("_all_");
        for ( idx ii=0; ii<ids->dim(); ++ii) *pcolsetset->add()=ii;
    }
    return lcolset->dim();
}

idx sText::categoryListInverse(sDic < sVec < idx > >  * lcolset , sVec <idx> * nocolset, sDic < idx > * ids, idx maxRows)
{
    if(ids)maxRows=ids->dim();

    // now choose all the roows which are not present in any colset 
    for( idx ir=0, il=0, ic=0 ; ir<maxRows; ++ir )  { 
        idx lookFor=ids ? (*ids)[ir] : ir;

        for( il=0 ; il < lcolset->dim() ; ++ il ) {
            
            for( ic=0 ; ic < lcolset[il].dim() ; ++ic )  { 
                if( (*lcolset)[il][ic]==lookFor ) 
                    break;
            }
            if(ic< (*lcolset)[il].dim())break;
        }
        if(il>=lcolset->dim() )
            nocolset->vadd(1,lookFor);
    }
                
    return nocolset->dim();
    //return learnRows.dim();
}


void sText::categoryListToDic( sDic < sVec < idx > >  * pcolset, sDic < idx > * ids)
{
    for( idx ic=0; ic<pcolset->dim() ; ++ic) {
        sVec < idx > * curset=pcolset->ptr(ic);
    
        for( idx is=0; is<curset->dim() ; ++is) {
            //idx aa=(*curset)[is];
            *ids->ptr((*curset)[is])=ic;
        }
    }
}


void sText::categoryListOut(sStr * str, sDic < sDic < sVec < idx > >  > * lcolset , sDic < idx > * ids)
{
    for( idx icat=0; icat<lcolset->dim() ; ++icat) {
        str->printf("Categorization %s\n", static_cast<const char *>(lcolset->id(icat)));
        sDic < sVec < idx > > * pcolset=lcolset->ptr(icat);
        
        for( idx ic=0; ic<pcolset->dim() ; ++ic) {
            str->printf("%s: ", static_cast<const char *>(pcolset->id(ic)));
            sVec < idx > * curset=pcolset->ptr(ic);
        
            for( idx is=0; is<curset->dim() ; ++is) {
                if(ids)str->printf("%s[%" DEC "]  ", static_cast<const char *>(ids->id((*curset)[is])), (*curset)[is]);
                else str->printf("%" DEC "  ", (*curset)[is]);
            }
            str->printf("\n");        
        }
        str->printf("\n");
    }
    //return learnRows.dim();
}
