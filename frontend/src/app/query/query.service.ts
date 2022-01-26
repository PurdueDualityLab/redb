import { Injectable } from '@angular/core';
import { HttpClient, HttpErrorResponse } from '@angular/common/http';
import { Query } from './query.model';
import { Observable, of } from 'rxjs';
import { map, catchError } from 'rxjs/operators';
import { ErrorService } from '../error/error.service';
import { ErrorState } from '../error/error-state';
import { environment } from '../../environments/environment';

@Injectable({
  providedIn: 'root'
})
export class QueryService {

  constructor(private readonly http: HttpClient, private readonly errorService: ErrorService) { }

  public query(query: Query): Observable<string[] | null> {
    return this.http.post<any>(`${environment.queryServiceUrl}/query`, query).pipe(
      map(body => body.results as string[]), // successfully handle the response
      catchError((err: HttpErrorResponse) => {
        let errorState: ErrorState = {
          title: "Error while performing query",
          message: `Error encountered: ${err.message}`,
          data: err
        };

        // Set the error and return null
        this.errorService.setErrorState(errorState);
        return of(null);
      })
    );
  }
}
