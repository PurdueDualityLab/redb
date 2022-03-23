import { HttpClient, HttpErrorResponse } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable, of } from 'rxjs';
import { catchError } from 'rxjs/operators';
import { environment } from 'src/environments/environment';
import { ErrorState } from '../error/error-state';
import { ErrorService } from '../error/error.service';
import { QueryEvent } from './tracking-info.model';

@Injectable({
  providedIn: 'root'
})
export class TrackingService {

  constructor(
    private readonly http: HttpClient,
    private readonly errorService: ErrorService
  ) {}

  reportQueryEvent(event: QueryEvent): Observable<any> {
    // TODO
    console.log(event);

    // Run the request
    return this.http.post<any>(`${environment.queryServiceUrl}/track`, event).pipe(
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
