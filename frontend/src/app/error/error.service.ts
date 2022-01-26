import { Injectable } from '@angular/core';
import { BehaviorSubject } from 'rxjs';
import { ErrorState } from './error-state';

@Injectable({
  providedIn: 'root'
})
export class ErrorService {

  errorState$ = new BehaviorSubject<ErrorState>({ title: 'Okay', message: 'Everything is okay!' });
  constructor() { }

  setErrorState(state: ErrorState) {
    this.errorState$.next(state);
  }

  setErrorTitle(title: string): void {
    const currentErrorState = this.errorState$.getValue();
    this.errorState$.next({ ...currentErrorState, title });
  }

  setErrorMessage(msg: string): void {
    const currentErrorState = this.errorState$.getValue();
    this.errorState$.next({ ...currentErrorState, message: msg });
  }

  setErrorData(data: any): void {
    const currentErrorState = this.errorState$.getValue();
    this.errorState$.next({ ...currentErrorState, data });
  }
}
